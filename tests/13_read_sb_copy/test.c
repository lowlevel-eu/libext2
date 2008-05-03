#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test: read_sb_copy
 * Autor: Antoine Kaufmann
 * Zweck: ext2_sb_read_copy testen
 */
int run_test(ext2_fs_t* fs)
{
    ext2_superblock_t sb_orig;
    ext2_superblock_t sb;

    if (!ext2_sb_read(fs, &sb_orig)) {
        test_error("Original Superblock konnte nicht eingelesen werden");
    }

    fs->sb = &sb_orig;
    if (!ext2_sb_read_copy(fs, &sb, 1)) {
        test_error("Superblockkopie konnte nicht eingelesen werden");
    }

    return 0;
}
