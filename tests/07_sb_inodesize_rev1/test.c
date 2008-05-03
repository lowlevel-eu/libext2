#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test: sb_inode_size_rev0
 * Autor: Antoine Kaufmann
 * Zweck: ext2_sb_inodesize Testen
 */
int run_test(ext2_fs_t* fs)
{
    ext2_superblock_t sb;
    int size;

    if (!ext2_sb_read(fs, &sb)) {
        test_error("Superblock konnte nicht eingelesen werden");
    }

    size = ext2_sb_inodesize(&sb);
    if (size != 256) {
        test_error("Errechnete Inodegroesse (%d) stimmt nicht", size);
    }

    return 0;
}
