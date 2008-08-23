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

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <ext2.h>
#include <etool.h>

static ext2_fs_t fs_mem;
ext2_fs_t* fs = &fs_mem;

static int fd;

/**
 * Cache-Struktur fuer libext2-Caches
 */
typedef struct cache {
    size_t block_size;
} cache_t;



static int dev_read(uint64_t start, size_t size, void* dest, void* prv)
{
    if (lseek(fd, (off_t) start, SEEK_SET) == (off_t) -1) {
        return 0;
    }

    return (read(fd, dest, size) == size);
}

static int dev_write(uint64_t start, size_t size, const void* dest, void* prv)
{
    printf("write to %0llx 0x%llx bytes\n", (unsigned long long) start,
        (unsigned long long) size);
    if (lseek(fd, (off_t) start, SEEK_SET) == (off_t) -1) {
        return 0;
    }

    return (write(fd, dest, size) == size);
}

static void* cache_create(struct ext2_fs* fs, size_t block_size)
{
    cache_t* c = malloc(sizeof(c));
    c->block_size = block_size;
    return c;
}

static void cache_destroy(void* handle)
{
    cache_t* c = handle;
    free(c);
}

static ext2_cache_block_t* cache_block(void* handle, uint64_t block, int noread)
{
    cache_t* c = handle;
    ext2_cache_block_t* b = malloc(sizeof(*b));
    size_t bs = c->block_size;

    b->data = malloc(bs);
    if (!noread) {
        dev_read(block * bs, bs, b->data, NULL);
    }
    b->cache = handle;
    b->number = block;
    return b;
}

static void cache_block_dirty(ext2_cache_block_t* b)
{
    cache_t* c = b->cache;
    size_t bs = c->block_size;

    dev_write(b->number * bs, bs, b->data, NULL);
}

static void cache_block_free(ext2_cache_block_t* b, int dirty)
{
    if (dirty) {
        cache_block_dirty(b);
    }

    free(b->data);
    free(b);
}


int etool_img_init(const char* img)
{
    fd = open(img, O_RDWR | O_EXCL);

    if (fd == -1) {
        perror("Image konnte nicht geoffnet werden");
        return 0;
    }

    fs->dev_read = dev_read;
    fs->dev_write = dev_write;

    fs->cache_create = cache_create;
    fs->cache_destroy = cache_destroy;
    fs->cache_block = cache_block;
    fs->cache_block_dirty = cache_block_dirty;
    fs->cache_block_free = cache_block_free;

    if (!ext2_fs_mount(fs)) {
        fprintf(stderr, "Mounten fehlgeschlagen\n");
        close(fd);
        return 0;
    }

    return 1;
}

int etool_img_close()
{
    fsync(fd);
    close(fd);
    return 1;
}
