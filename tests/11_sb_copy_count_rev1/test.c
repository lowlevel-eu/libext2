#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test: sb_copy_count
 * Autor: Antoine Kaufmann
 * Zweck: ext2_sb_copy_count testen
 */
int run_test(ext2_fs_t* fs)
{
    ext2_superblock_t sb;

    if (!ext2_sb_read(fs, &sb)) {
        test_error("Superblock konnte nicht eingelesen werden");
    }

    if (ext2_sb_copy_count(&sb) != 2) {
        test_error("Anzahl der Superblockkopien stimmt nicht");
    }

    return 0;
}
