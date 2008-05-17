/*
 * Copyright (c) 2008 The LOST Project. All rights reserved.
 *
 * This code is derived from software contributed to the LOST Project
 * by Kevin Wolf.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *     This product includes software developed by the LOST Project
 *     and its contributors.
 * 4. Neither the name of the LOST Project nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <string.h>
#include "ext2.h"
#include <stdio.h>

struct private_dir_get {
    char* name;
    size_t name_len;
    ext2_dirent_t* result;
    int found;
};

void ext2_dir_foreach(ext2_fs_t* fs, ext2_inode_t* inode,
    int (*handler) (ext2_dirent_t*, void*), void* private)
{
    ext2_dirent_t* entry;
    char buf[inode->raw.size];
    uint64_t pos = 0;

    ext2_inode_readdata(inode, 0, inode->raw.size, buf);
    while (pos < inode->raw.size) {
        entry = (ext2_dirent_t*) &buf[pos];

        if (handler(entry, private)) {
            break;
        }

        if (entry->record_len == 0) {
            // TODO: Was geschieht mit dem kaputten FS?
            break;
        }
        pos += entry->record_len;
    }
}

static int dir_get_handler(ext2_dirent_t* dirent, void* prv)
{
    struct private_dir_get* p = prv;

    if (p->name_len != dirent->name_len) {
        return 0;
    }

    if (!strncmp(p->name, dirent->name, p->name_len)) {
        p->result = malloc(dirent->record_len);
        memcpy(p->result, dirent, dirent->record_len);
        return 1;
    }

    return 0;
}

ext2_dirent_t* ext2_dir_get(ext2_fs_t* fs, ext2_inode_t* inode,
    char* name)
{
    struct private_dir_get prv = {
        .name = name,
        .name_len = strlen(name),
        .result = NULL
    };

    ext2_dir_foreach(fs, inode, dir_get_handler, &prv);
    return prv.result;
}

char* ext2_dir_alloc_name(ext2_dirent_t* dirent)
{
    char* buf = malloc(dirent->name_len + 1);
    memcpy(buf, dirent->name, dirent->name_len);
    buf[dirent->name_len] = '\0';
    return buf;
}

int ext2_dir_find(ext2_fs_t* fs, const char* path, ext2_inode_t* inode)
{
    // Puffer um den Pfad veraendern zu koennen
    char pb[strlen(path) + 1];
    const char* delim = "/";
    char* part;
    uint64_t parent_no = 1;
    ext2_inode_t parent;
    ext2_dirent_t* dirent;

    strcpy(pb, path);
    part = strtok(pb, delim);

    while (part) {
        if (strlen(part) != 0) {
            if (!ext2_inode_read(fs, parent_no, &parent)) {
                return 0;
            }

            dirent = ext2_dir_get(fs, &parent, part);
            if (!dirent) {
                return 0;
            }

            parent_no = dirent->inode - 1;
            free(dirent);
        }
        part = strtok(NULL, delim);
    }

    return ext2_inode_read(fs, parent_no, inode);
}

static size_t dirent_size(size_t name_len)
{
    size_t size = name_len + sizeof(ext2_dirent_t);

    // Auf 4 runden
    if ((size % 4) != 0) {
        size += 4 - (size % 4);
    }

    return size;
}


int ext2_dir_link(ext2_inode_t* dir, ext2_inode_t* inode, const char* name)
{
    ext2_dirent_t* entry;
    ext2_dirent_t* newentry;
    char buf[dir->raw.size];
    uint64_t pos = 0;
    size_t newentry_len = dirent_size(strlen(name));
    size_t entry_len = 0;

    if (dir->raw.size) {
    ext2_inode_readdata(dir, 0, dir->raw.size, buf);
    while (dir->raw.size && (pos < dir->raw.size)) {
        entry = (ext2_dirent_t*) &buf[pos];
        entry_len = dirent_size(entry->name_len);

        if (entry->record_len == 0) {
            // TODO: Was geschieht mit dem kaputten FS?
            break;
        }

        // Pruefen ob dieser Eintrag soviel zu lang ist, wie wir speicher
        // benoetigen
        if (entry->record_len - entry_len >= newentry_len) {
            newentry = (ext2_dirent_t*) &buf[pos + entry_len];

            // Neuen Eintrag befuellen
            newentry->name_len = strlen(name);
            memcpy(newentry->name, name, newentry->name_len);
            newentry->inode = inode->number + 1;
            newentry->record_len = entry->record_len - entry_len;

            // Alten Eintrag kuerzen
            entry->record_len = entry_len;

            ext2_inode_writedata(dir, pos, entry_len + newentry->record_len,
                buf + pos);

            // Referenzzaehler erhoehen
            inode->raw.link_count++;
            return 1;
        }
        pos += entry->record_len;
    }
    }
    // TODO: Hier koennten eventuell noch ein paar Bytes vom vorherigen Eintrag
    // genommen werden.
    {
        char newbuf[newentry_len];
        newentry = (ext2_dirent_t*) newbuf;

        newentry->name_len = strlen(name);
        memcpy(newentry->name, name, newentry->name_len);
        newentry->inode = inode->number + 1;
        newentry->record_len = ext2_sb_blocksize(dir->fs->sb);
        ext2_inode_writedata(dir, pos, newentry->record_len, newbuf);
        inode->raw.link_count++;
        return 1;
    }
    return 0;
}


int ext2_dir_create(ext2_inode_t* parent, const char* name, ext2_inode_t* newi)
{
    ext2_blockgroup_t bg;

    // Neuen Inode alloziern
    if (!ext2_inode_alloc(parent->fs, newi)) {
        return 0;
    }

    // Verzeichniseintraege anlegen
    if (!ext2_dir_link(parent, newi, name) ||
        !ext2_dir_link(newi, newi, ".") ||
        !ext2_dir_link(newi, parent, ".."))
    {
        return 0;
    }


    newi->raw.deletion_time = 0;
    newi->raw.mode = EXT2_INODE_MODE_DIR;
    if (!ext2_inode_update(newi) ||
        !ext2_inode_update(parent))
    {
        return 0;
    }

    // Verzeichnisanzahl in der Blockgruppe erhoehen
    ext2_bg_read(parent->fs, 0, &bg);
    bg.used_directories++;
    ext2_bg_update(parent->fs, 0, &bg);

    return 1;
}

