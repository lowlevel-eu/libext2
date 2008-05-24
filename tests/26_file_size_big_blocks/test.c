#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test:  file_size_big_blocks
 * Autor: Antoine Kaufmann
 * Zweck: Groesse des Rootverzeichnisses lesen, mit einem Dateisystem mit 2048
 *        Bytes grossen Blocks.
 */
int run_test(ext2_fs_t* fs)
{
    ext2_inode_t rootdir;
    rootdir.raw.size = 0;

    ext2_fs_mount(fs);

    if (!ext2_inode_read(fs, EXT2_ROOTDIR_INODE, &rootdir)) {
        test_error("Inode konnte nicht eingelesen werden");
    }

    if (rootdir.raw.size != 2048) {
        test_error("Eingelesene Groesse (%d) stimmt nicht", rootdir.raw.size);
    }

    return 0;
}
