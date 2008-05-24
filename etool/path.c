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

#include <stdlib.h>
#include <string.h>
#include <etool.h>

char* etool_basename(const char* path)
{
    const char* basename_start;
    char* basename;

    basename_start = strrchr(path, '/');

    // Wenn keine Slashes vorkommen ist der basename einfach der uebergebene
    // Pfad
    if (!basename_start) {
        basename_start = path;
    } else {
        basename_start++;
    }

    basename = malloc(strlen(basename_start) + 1);
    strcpy(basename, path);

    return basename;
}

char* etool_dirname(const char* path)
{
    char* dirname_end;
    size_t dirname_len;
    char* dirname;

    dirname_end = strrchr(path, '/');

    // Wenn kein slash vorhanden ist, ist der basename .
    if (!dirname_end) {
        dirname = malloc(1 + 1);
        strcpy(dirname, ".");
    } else {
        if (dirname_end != path) {
            // Letzten Slash abtrennen, falls es sich nicht um das erste Zeichen
            // handelt
            dirname_end--;
        }

        dirname_len = dirname_end - path;
        dirname = malloc(dirname_len + 1);
        strncpy(dirname, path, dirname_len);
        dirname[dirname_len] = 0;
    }

    return dirname;
}

