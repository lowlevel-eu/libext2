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

static void mkdir_usage()
{
    puts("Aufruf: etool <image> mkdir <Verzeichnis>");
}

static int mkdir(char* path)
{
    char* dirname = etool_dirname(path);
    char* basename = etool_basename(path);
    ext2_inode_t dir_inode;
    ext2_inode_t new_inode;
    int result = 0;

    if (!ext2_dir_find(fs, dirname, &dir_inode)) {
        fprintf(stderr, "Fehler: Verzeichnis nicht gefunden: '%s'!\n", dirname);
        goto out_err;
    }

    if (!EXT2_INODE_IS_DIR(&dir_inode)) {
        fprintf(stderr, "Fehler: Kein Verzeichnis: '%s'!\n", dirname);
        goto out_err;
    }

    if (!ext2_dir_create(&dir_inode, basename, &new_inode)) {
        fprintf(stderr, "Fehler: Verzeichnis '%s' konnte nicht erstellt werden "
            "in '%s'!\n", basename, dirname);
        goto out_err;
    }

    ext2_inode_dump(&new_inode);
    goto out;

out_err:
    result = -1;
out:
    free(dirname);
    free(basename);
    return result;
}

int etool_mkdir(int argc, char* argv[])
{
    if (argc < 1) {
        mkdir_usage();
        return -1;
    }

    return mkdir(argv[1]);
}

