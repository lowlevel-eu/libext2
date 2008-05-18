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

#include <etool.h>
#include <ext2.h>
#include <check.h>

static int dirent_type(ext2_dirent_t* dirent, ext2_inode_t* inode)
{
    int expected = ext2_inode_type(inode);

    if (!(fs->sb->features_incompat & EXT2_FEATURE_INCOMP_FILETYPE)) {
        return 1;
    }

    if (expected != dirent->type) {
        fprintf(stderr, "Fehler: Verzeichniseintrag hat nicht den erwarteten "
            "Typ (erwartet %d; typ %d)\n", expected, dirent->type);
        return 0;
    }

    return 1;
}

static int check_dir(ext2_inode_t* inode)
{
    int errors = 0;

    if (!EXT2_INODE_IS_DIR(inode)) {
        return 1;
    }

    int handler(ext2_dirent_t* dirent, void* priv) {
        ext2_inode_t inode;
        int* errors = priv;

        if (!ext2_inode_read(fs, dirent->inode - 1, &inode)) {
            fprintf(stderr, "Fehler: Root-Inode konnte nicht eingelesen werden!"
                "\n");
            return 1;
        }

        if (!EXT2_INODE_IS_DIR(&inode)) {
            return 0;
        }

        if (!dirent_type(dirent, &inode)) {
            (*errors)++;
            return 1;
        }

        return 0;
    }

    ext2_dir_foreach(fs, inode, handler, &errors);

    return (errors == 0);
}

int check_directories()
{
    ext2_inode_t root_dir;

    if (!ext2_inode_read(fs, 1, &root_dir)) {
        fprintf(stderr, "Fehler: Root-Inode konnte nicht eingelesen werden!\n");
        return -1;
    }

    return !check_dir(&root_dir);
}

