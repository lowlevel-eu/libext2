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

struct imgpriv {
    ext2_inode_t inode;
    uint64_t pos;
};



static int img_exists(const char* path)
{
    ext2_inode_t inode;

    return ext2_dir_find(fs, path, &inode);
}

static int img_is_dir(const char* path)
{
    ext2_inode_t inode;

    ext2_dir_find(fs, path, &inode);
    return EXT2_INODE_IS_DIR(&inode);
}


static int img_create(const char* full, const char* dir, const char* base,
    void** private)
{
    ext2_inode_t dir_inode;
    ext2_inode_t new_inode;

    if (!ext2_dir_find(fs, dir, &dir_inode)) {
        return 0;
    }

    return ext2_file_create(&dir_inode, base, &new_inode);
}

static int img_open(const char* path, int write, void** private)
{
    struct imgpriv* ip;

    *private = malloc(sizeof(*ip));
    ip = *private;

    // TODO: Truncate
    ip->pos = 0;
    return ext2_dir_find(fs, path, &ip->inode);
}

static int img_close(void** private)
{
    struct imgpriv* ip = *private;
    return ext2_inode_update(&ip->inode);
}

static uint64_t img_size(void** private)
{
    struct imgpriv* ip = *private;
    return ip->inode.raw->size;
}

static int img_read(void* buf, int size, void** private)
{
    struct imgpriv* ip = *private;

    if ((uint64_t) size > ip->inode.raw->size - ip->pos) {
        size = ip->inode.raw->size - ip->pos;
    }

    if (!ext2_inode_readdata(&ip->inode, ip->pos, size, buf)) {
        return -1;
    }

    ip->pos += size;
    return size;
}

static int img_write(void* buf, int size, void** private)
{
    struct imgpriv* ip = *private;

    if (!ext2_inode_writedata(&ip->inode, ip->pos, size, buf)) {
        return -1;
    }

    ip->pos += size;
    return size;
}



struct drv image_drv = {
    .exists = img_exists,
    .is_dir = img_is_dir,
    .create = img_create,
    .open = img_open,
    .close = img_close,
    .size = img_size,
    .read = img_read,
    .write = img_write
};

