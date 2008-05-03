#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test:  file_size
 * Autor: Kevin Wolf
 * Zweck: Groesse des Rootverzeichnisses testen
 */
int run_test(ext2_fs_t* fs)
{
    ext2_inode_t rootdir;
    rootdir.size = 0;

    if (!ext2_inode_read(fs, 1, &rootdir)) {
        test_error("Inode konnte nicht eingelesen werden");
    }

    if (rootdir.size != 1024) {
        test_error("Eingelesene Groesse (%d) stimmt nicht", rootdir.size);
    }

    return 0;
}
