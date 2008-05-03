#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test:  read_broken_superblock
 * Autor: Kevin Wolf
 * Zweck: ext2_read_superblock testen. Das Image enthaelt keinen gueltigen
 *        Superblock und soll deswegen zurueckgewiesen werden
 */
int run_test(ext2_fs_t* fs)
{
    ext2_superblock_t sb;

    if (ext2_sb_read(fs, &sb)) {
        test_error("Superblock ist kaputt, wurde aber trotzdem eingelesen");
    }

    return 0;
}
