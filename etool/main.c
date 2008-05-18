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
#include <string.h>
#include <etool.h>


struct tool {
    const char* name;
    int (*f)(int argc, char* argv[]);
};

/**
 * Liste mit den Funktionen und ihren Namen wie sie von aussen angesprochen
 * werden koennen
 */
struct tool tools[] = {
    {"ls", etool_ls},
    {"cat", etool_cat},
    {"check", etool_check},
    {NULL, NULL}
};

/**
 * Tool anhand des Namens suchen
 */
static struct tool* get_tool(const char* name)
{
    int i;
    struct tool* tool;

    for (i = 0; (tool = tools + i) && tool->name; i++) {
        if (strcmp(tool->name, name) == 0) {
            return tool;
        }
    }

    return NULL;
}

/**
 * Hilfe zur Benutzung anzeigen
 */
static void display_usage()
{
    printf("Benutzung: etool <Image> <Tool> [Argumente]\n");
}

int main(int argc, char* argv[]) {
    struct tool* tool;
    const char* tool_name = argv[2];
    int result;

    if (argc < 3) {
        display_usage();
        return -1;
    }

    if (!(tool = get_tool(tool_name))) {
        printf("Unbekanntes Tool '%s'\n", tool_name);
        return -1;
    }
    
    if (!etool_img_init(argv[1])) {
        return -1;
    }

    result = tool->f(argc - 2, argv + 2);

    etool_img_close();

    return result;
}
