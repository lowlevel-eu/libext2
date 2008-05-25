#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test:  inode_read_data
 * Autor: Kevin Wolf
 * Zweck: inode_read_data testen
 */
int run_test(ext2_fs_t* fs)
{
    ext2_inode_t rootdir;
    ext2_superblock_t sb;

    ext2_fs_mount(fs);

    if (!ext2_sb_read(fs, &sb)) {
        test_interror("Superblock konnte nicht eingelesen werden");
    }

    if (!ext2_inode_read(fs, 1, &rootdir)) {
        test_interror("Inode konnte nicht eingelesen werden");
    }

    size_t blksize = ext2_sb_blocksize(&sb);
    char blkbuf[2 * blksize];
    char databuf[2 * blksize];
    ext2_inode_readblk(&rootdir, 0, blkbuf);
    ext2_inode_readblk(&rootdir, 1, blkbuf + blksize);

    if (!ext2_inode_readdata(&rootdir, 0, blksize, databuf)) {
        test_error("Fehler beim Einlesen der Daten");
    }
    if (memcmp(databuf, blkbuf, blksize)) {
        test_error("databuf != blkbuf (0) fuer blksize, aligned");
    }

    ext2_inode_readdata(&rootdir, blksize, blksize, databuf);
    if (memcmp(databuf, blkbuf + blksize, blksize)) {
        test_error("databuf != blkbuf (1) fuer blksize, aligned");
    }

    ext2_inode_readdata(&rootdir, blksize / 2, blksize, databuf);
    if (memcmp(databuf, blkbuf + blksize / 2, blksize)) {
        test_error("databuf != blkbuf fuer blksize, start und ende nicht "
            "aligned");
    }

    ext2_inode_readdata(&rootdir, blksize / 2, blksize / 2, databuf);
    if (memcmp(databuf, blkbuf + blksize / 2, blksize / 2)) {
        test_error("databuf != blkbuf fuer blksize, start nicht aligned");
    }

    ext2_inode_readdata(&rootdir, blksize, blksize / 2, databuf);
    if (memcmp(databuf, blkbuf + blksize, blksize / 2)) {
        test_error("databuf != blkbuf fuer blksize, ende nicht aligned");
    }


    memset(databuf, 0xff, 42);
    memset(databuf + 42 + 64, 0xee, blksize - 42 - 64);
    memcpy(databuf + blksize, databuf, blksize);
    ext2_inode_readdata(&rootdir, 42, 64, databuf + 42);

    if (memcmp(databuf + 42, blkbuf + 42, 64)) {
        test_error("databuf != blkbuf fuer 64 Bytes, nicht aligned");
    }
    if (memcmp(databuf, databuf + blksize, 42)) {
        test_error("Beginn von databuf ueberschrieben, 64 Bytes");
    }
    if (memcmp(
            databuf + 42 + 64,
            databuf + blksize + 42 + 64,
            blksize - 42 - 64))
    {
        test_error("Ende von databuf ueberschrieben, 64 Bytes");
    }

    return 0;
}
