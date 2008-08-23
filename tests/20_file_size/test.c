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
    rootdir.raw.size = 0;

    ext2_fs_mount(fs);

    if (!ext2_inode_read(fs, EXT2_ROOTDIR_INODE, &rootdir)) {
        test_error("Inode konnte nicht eingelesen werden");
    }

    if (rootdir.raw.size != 1024) {
        test_error("Eingelesene Groesse (%d) stimmt nicht", rootdir.raw.size);
    }

    if (!ext2_fs_unmount(fs)) {
        test_interror("Unmounten fehlgeschlagen");
    }

    return 0;
}
