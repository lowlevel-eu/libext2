#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test:  create_dir_rev1
 * Autor: Antoine Kaufmann
 * Zweck: Neues Verzeichnis anlegen auf einem Rev1-Dateisystem
 */
int run_test(ext2_fs_t* fs)
{
    ext2_inode_t rootdir;
    ext2_inode_t newdir;

    if (!ext2_fs_mount(fs)) {
        test_interror("Mounten fehlgeschlagen");
    }

    if (!ext2_inode_read(fs, EXT2_ROOTDIR_INODE, &rootdir)) {
        test_interror("Fehler beim Lesen des Rootverzeichnis-Inode");
    }

    if (!ext2_dir_create(&rootdir, "newdir", &newdir)) {
        test_error("Fehler beim Anlegen des Verzeichnisses");
    }

    if (!ext2_fs_unmount(fs)) {
        test_interror("Unmounten fehlgeschlagen");
    }

    test_etool();
    test_fsck();

    return 0;
}
