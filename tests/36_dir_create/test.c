#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test:  create_dir
 * Autor: Antoine Kaufmann
 * Zweck: Neues Verzeichnis anlegen
 */
int run_test(ext2_fs_t* fs)
{
    ext2_inode_t rootdir;
    ext2_inode_t newdir;

    if (!ext2_fs_mount(fs)) {
        test_interror("Mounten fehlgeschlagen");
    }

    if (!ext2_inode_read(fs, 1, &rootdir)) {
        test_interror("Fehler beim Lesen des Rootverzeichnis-Inode");
    }

    if (!ext2_dir_create(&rootdir, "newdir", &newdir)) {
        test_error("Fehler beim Anlegen des Verzeichnisses");
    }

    test_fsck();

    return 0;
}
