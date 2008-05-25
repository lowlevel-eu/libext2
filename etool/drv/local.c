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
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <etool.h>
#include <ext2.h>

struct localpriv {
    int fd;
};

static int local_exists(const char* path)
{
    struct stat buf;

    return !stat(path, &buf);
}

static int local_is_dir(const char* path)
{
    struct stat buf;
    stat(path, &buf);

    return S_ISDIR(buf.st_mode);
}

static int local_create(const char* full, const char* dir, const char* base,
    void** private)
{
    int fd = open(full, O_WRONLY | O_CREAT | O_EXCL, 0660);

    if (fd == -1) {
        return 0;
    }

    close(fd);
    return 1;
}

static int local_open(const char* path, int write, void** private)
{
    struct localpriv* lp;
    int flags;
    int fd;

    if (write) {
        flags = O_WRONLY | O_TRUNC;
    } else {
        flags = O_RDONLY;
    }

    fd = open(path, flags);

    if (fd != -1) {
        puts("succ");
        *private = malloc(sizeof(*lp));
        lp = *private;
        lp->fd = fd;
    } else {
        perror("blub");
    }

    return (fd != -1);
}

static int local_close(void** private)
{
    struct localpriv* lp = *private;
    close(lp->fd);

    free(lp);

    return 1;
}

static uint64_t local_size(void** private)
{
    struct localpriv* lp = *private;
    struct stat buf;

    fstat(lp->fd, &buf);
    return buf.st_size;
}

static int local_read(void* buf, int size, void** private)
{
    struct localpriv* lp = *private;
    return read(lp->fd, buf, size);
}

static int local_write(void* buf, int size, void** private)
{
    struct localpriv* lp = *private;
    return write(lp->fd, buf, size);
}



struct drv local_drv = {
    .exists = local_exists,
    .is_dir = local_is_dir,
    .create = local_create,
    .open = local_open,
    .close = local_close,
    .size = local_size,
    .read = local_read,
    .write = local_write
};

