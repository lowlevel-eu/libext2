/*
 * Copyright (c) 2008 The LOST Project. All rights reserved.
 *
 * This code is derived from software contributed to the LOST Project
 * by Kevin Wolf.
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

#include <ext2.h>

int ext2_bg_read(ext2_fs_t* fs, int group_nr, ext2_blockgroup_t* bg)
{
    if (!fs->dev_read(2048, sizeof(ext2_blockgroup_t), bg, fs->dev_private)) {
        return 0;
    }

    return 1;
}

int ext2_inode_read(ext2_fs_t* fs, uint64_t inode_nr, ext2_inode_t* inode)
{
    size_t inode_size;
    uint64_t inode_offset;

    ext2_superblock_t sb;
    ext2_blockgroup_t bg;

    ext2_sb_read(fs, &sb);
    ext2_bg_read(fs, 0, &bg);

    inode_size = ext2_sb_inodesize(&sb);
    inode_offset = ext2_sb_blocksize(&sb) * bg.inode_table
        + inode_size * inode_nr;

    if (!fs->dev_read(inode_offset, sizeof(*inode), inode, fs->dev_private)) {
        return 0;
    }

    return 1;
}

int ext2_inode_readblk(ext2_fs_t* fs, ext2_inode_t* inode, uint64_t block,
    void* buf)
{
    ext2_superblock_t sb;
    ext2_sb_read(fs, &sb);

    size_t   block_size = ext2_sb_blocksize(&sb);
    uint64_t block_nr = inode->blocks[block];
    uint64_t block_offset = block_size * block_nr;

    if (!fs->dev_read(block_offset, block_size, buf, fs->dev_private)) {
        return 0;
    }

    return 1;
}
