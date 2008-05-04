#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test:  inode_read_data_sparse
 * Autor: Antoine Kaufmann
 * Zweck: inode_read_data testen mit Sparse File
 */
int run_test(ext2_fs_t* fs)
{
    ext2_inode_t rootdir;

    ext2_fs_mount(fs);
    rootdir.size = 0;

    if (!ext2_inode_read(fs, 1, &rootdir)) {
        test_interror("Inode konnte nicht eingelesen werden");
    }

    size_t blksize = ext2_sb_blocksize(fs->sb);
    char buf[blksize];
    char cmp_buf[blksize];

    memset(buf, 0xFF, blksize);
    memset(cmp_buf, 0x0, blksize);

    ext2_inode_readdata(fs, &rootdir, 0, blksize, buf);
    if (memcmp(buf, cmp_buf, blksize)) {
        test_error("buf != cmp_buf");
    }

    return 0;
}

