/*
 * Copyright (c) 2007 The LOST Project. All rights reserved.
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
#include <ext2.h>
#include <fcntl.h>

static int image_fd;

static int dev_read(uint64_t start, size_t size, void* dest)
{
    if (lseek(image_fd, (off_t) start, SEEK_SET) == (off_t) -1) {
        return 0;
    }

    return (read(image_fd, dest, size) == size);
}

static int dev_write(uint64_t start, size_t size, void* dest)
{
    if (lseek(image_fd, (off_t) start, SEEK_SET) == (off_t) -1) {
        return 0;
    }

    return (write(image_fd, dest, size) == size);
}


extern int run_test(ext2_fs_t* fs);
int main(int argc, char* argv[])
{
    ext2_fs_t fs;

    printf("Test %s:\n", argv[0]);

    fs.dev_read = dev_read;
    fs.dev_write = dev_write;

    image_fd = open(argv[1], O_RDWR);
    if (image_fd == -1) {
        puts("  Fehler beim Oeffnen des Images");
        return -1;
    }

    run_test(&fs);
    printf("  Test [\033[01;32mpassed\033[00;00m]\n");

    // Sonst bricht make ab
    return 0;
}

