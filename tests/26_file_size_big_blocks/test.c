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

    ext2_fs_mount(fs);

    if (!ext2_inode_read(fs, EXT2_ROOTDIR_INODE, &rootdir)) {
        test_error("Inode konnte nicht eingelesen werden");
    }

    if (rootdir.raw->size != 2048) {
        test_error("Eingelesene Groesse (%d) stimmt nicht", rootdir.raw->size);
    }

    ext2_inode_release(&rootdir);
    if (!ext2_fs_unmount(fs)) {
        test_interror("Unmounten fehlgeschlagen");
    }

    return 0;
}
