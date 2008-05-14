/*
 * Copyright (c) 2008 The LOST Project. All rights reserved.
 *
 * This code is derived from software contributed to the LOST Project
 * by Antoine Kaufmann.
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

#include <stdio.h>
#include <unistd.h>
#include <etool.h>
#include <ext2.h>

static char* prefix = "";
static int f_show_all = 0;
static int f_show_size = 0;


static int handle_dirent(ext2_dirent_t* dirent, void* prv)
{
    int result = 0;
    char* name = ext2_dir_alloc_name(dirent);
    ext2_inode_t inode;

    // Versteckte eintraege nur anzeigen, wenn -a angegeben wurde
    if (!f_show_all && (name[0] == '.')) {
        goto end;
    }

    if (!ext2_inode_read(fs, dirent->inode, &inode)) {
        fprintf(stderr, "Fehler: Inode von '%s'konnte nicht eingelesen werden!"
            "\n", name);
        result = -1;
        goto end;
    }

    printf("%s", prefix);

    if (f_show_size) {
        printf("%d ", inode.raw.size);
    }

    printf("%s\n", name);

end:
    free(name);
    return 0;
}

static void ls(const char* path)
{
    ext2_inode_t inode;

    if (!ext2_dir_find(fs, path, &inode)) {
        fprintf(stderr, "Fehler: Pfad nicht gefunden!\n");
        return;
    }

    if (!EXT2_INODE_IS_DIR(&inode)) {
        fprintf(stderr, "Fehler: Kein Verzeichnis!\n");
        return;
    }

    ext2_dir_foreach(fs, &inode, handle_dirent, NULL);
}

int etool_ls(int argc, char* argv[])
{
    char* path = "/";
    int multiple_dirs = 0;

    while (optind < argc) {
        int result = getopt(argc, argv, "as");
        if (result == -1) {
            break;
        }

        switch (result) {
            case 'a':
                f_show_all = 1;
                break;

            case 's':
                f_show_size = 1;
                break;

            case '?':
            default:
                fprintf(stderr, "Fehler: Unbekannter Parameter\n");
                return -1;

        }
    }

    // Wenn mehrere Verzeichnisse angezeigt werden sollen, ein paar
    // leerschlaege anzeigen
    if ((argc - optind) > 1) {
        prefix = "  ";
        multiple_dirs = 1;
    }

    // Wenn keine Argumente angegeben wurden, lese Root-Verzeichnis aus
    if ((argc - optind) == 0) {
        ls("/");
    }

    // Pfade verarbeiten
    while (optind < argc) {
        path = argv[optind++];

        if (multiple_dirs) {
            printf("%s:\n", path);
        }

        ls(path);
   }

    return 0;
}

