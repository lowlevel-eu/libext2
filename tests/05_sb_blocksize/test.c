#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test: sb_blocksize
 * Autor: Antoine Kaufmann
 * Zweck: ext2_sb_blocksize testen
 */
int run_test(ext2_fs_t* fs)
{
    ext2_superblock_t sb;
    int size;

    if (!ext2_sb_read(fs, &sb)) {
        test_error("Superblock konnte nicht eingelesen werden");
    }

    size = ext2_sb_blocksize(&sb);
    if (size != 1024) {
        test_error("Errechnete Blockgroesse (%d) stimmt nicht", size);
    }

    return 0;
}
