#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test: read_superblock
 * Autor: Antoine Kaufmann
 * Zweck: ext2_read_superblock testen
 */
int run_test(ext2_fs_t* fs)
{
    ext2_superblock_t sb;

    if (!ext2_superblock_read(fs, &sb)) {
        test_error("Superblock konnte nicht eingelesen werden");
    }

    if (sb.magic != EXT2_SB_MAGIC) {
        test_error("Eingelesene Magic stimmt nicht");
    }

    return 0;
}
