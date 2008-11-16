#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test:  readdata_nothing
 * Autor: Antoine Kaufmann
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
    char databuf[blksize];

    if (!ext2_inode_readdata(&rootdir, 0, 0, databuf)) {
        test_error("Fehler beim Einlesen der Daten");
    }

    ext2_inode_release(&rootdir);
    if (!ext2_fs_unmount(fs)) {
        test_interror("Unmounten fehlgeschlagen");
    }

    return 0;
}
