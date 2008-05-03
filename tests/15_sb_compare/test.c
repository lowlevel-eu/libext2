#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>


/**
 * Test: sb_compare
 * Autor: Antoine Kaufmann
 * Zweck: ext2_sb_compare testen
 */
int run_test(ext2_fs_t* fs)
{
    ext2_superblock_t sb;
    ext2_superblock_t sb_okay;
    ext2_superblock_t sb_bad;

    if (!ext2_sb_read(fs, &sb)) {
        test_error("Original Superblock konnte nicht eingelesen werden");
    }
    fs->sb = &sb;

    if (!ext2_sb_read_copy(fs, &sb_okay, 0)) {
        test_error("Superblockkopie konnte nicht eingelesen werden");
    }

    if (!ext2_sb_read_copy(fs, &sb_bad, 1)) {
        test_error("Superblockkopie konnte nicht eingelesen werden");
    }

    if (!ext2_sb_compare(&sb, &sb_okay)) {
        test_error("Vergleich 1 gescheitert");
    }

    if (ext2_sb_compare(&sb, &sb_bad)) {
        test_error("Vergleich 2 gescheitert");
    }

    return 0;
}
