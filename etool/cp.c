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
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <etool.h>
#include <ext2.h>
#define BUF_SIZE (16*1024)

static int cp(char* src, char* dest)
{
    char* dest_path = dest;
    char* src_basename;
    char* src_dirname;
    char* dest_basename;
    char* dest_dirname;
    struct drv* src_drv;
    struct drv* dest_drv;
    void* src_priv;
    void* dest_priv;
    char buf[BUF_SIZE];
    int result = 0;

    // Pruefen ob es sich um Pfade auf dem Image oder loakale handelt
    if (src[0] == ':') {
        src_drv = &image_drv;
        // : ueberspringen
        src++;
        puts("srcimg");
    } else {
        src_drv = &local_drv;
    }
    if (dest[0] == ':') {
        dest_drv = &image_drv;
        // : ueberspringen
        dest++;
        puts("destimg");
    } else {
        dest_drv = &local_drv;
    }

    // Dirname und basename fuer Ziel und Quelle zusammenbasteln
    src_basename = etool_basename(src);
    src_dirname = etool_dirname(src);

    // Der Zielpfad muss anders behandelt werden, wenn es sich dabei um ein
    // Verzeichnis handelt. Dann wird der ganze Pfad als dirname benutzt, und
    // fuer den Basename wird der der Quelldatei benutzt.
    if (dest_drv->exists(dest) && dest_drv->is_dir(dest)) {
        dest_dirname = malloc(strlen(dest) + 1);
        strcpy(dest_dirname, dest);

        dest_basename = malloc(strlen(src_basename) + 1);
        strcpy(dest_basename, src_basename);

        asprintf(&dest_path, "%s/%s", dest_dirname, dest_basename);
    } else {
        dest_basename = etool_basename(dest);
        dest_dirname = etool_dirname(dest);

        dest_path = malloc(strlen(dest) + 1);
        strcpy(dest_path, dest);
    }

    // Quelldatei oeffnen
    if (!src_drv->open(src, 0, &src_priv)) {
        fprintf(stderr, "Fehler: Die Quelldatei '%s' konnte nicht geoeffnen "
            "werden!\n", src);
        result = -1;
        goto out;
    }

    // Zieldatei anlegen falls notwendig
    if (!dest_drv->exists(dest_path)) {
        if (!dest_drv->create(dest_path, dest_dirname, dest_basename,
            &dest_priv))
        {
            fprintf(stderr, "Fehler: Zieldatei '%s' konnte nicht angelegt "
                "werden!\n", dest_path);
            result = -1;
            goto out;
        }
    }

    // Zieldatei oeffnen
    if (!dest_drv->open(dest_path, 1, &dest_priv)) {
        fprintf(stderr, "Fehler: Zieldatei '%s' konnte nicht geoeffnet "
            "werden!\n", dest_path);
        result = -1;
        goto out;
    }

    // Daten kopieren
    while ((result = src_drv->read(buf, BUF_SIZE, &src_priv))) {
        if (result == -1) {
            fprintf(stderr, "Fehler: Beim Einlesen der Daten ist etwas schief "
                "gegangen\n");
            break;
        }

        if (!dest_drv->write(buf, result, &dest_priv)) {
            fprintf(stderr, "Fehler: Schreiben von Daten nicht erfolgreich "
                "abgeschlossen!\n");
            result = -2;
            break;
        }
    }

    src_drv->close(&src_priv);
    dest_drv->close(&dest_priv);
out:
    free(dest_path);
    free(src_basename);
    free(src_dirname);
    free(dest_basename);
    free(dest_dirname);
    return 0;
}

int etool_cp(int argc, char* argv[])
{
    if (argc != 3) {
        puts("Benutzung: etool <Image> cp <Quelle> <Ziel>");
        puts("");
        puts("    Wenn die Pfade mit / oder direkt mit Buchstaben beginnen");
        puts("    werden sie als lokale Pfade gewertet.");
        puts("    Beginnen sie mit einem : wird von einem Pfad auf dem Image");
        puts("    ausgegangen.");
        return -1;
    }

    return cp(argv[1], argv[2]);
}

