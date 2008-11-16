#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test:  create_file
 * Autor: Antoine Kaufmann
 * Zweck: Neue Datei anlegen
 */
int run_test(ext2_fs_t* fs)
{
    ext2_inode_t rootdir;
    ext2_inode_t newfile;

    if (!ext2_fs_mount(fs)) {
        test_interror("Mounten fehlgeschlagen");
    }

    if (!ext2_inode_read(fs, EXT2_ROOTDIR_INODE, &rootdir)) {
        test_interror("Fehler beim Lesen des Rootverzeichnis-Inode");
    }

    if (!ext2_file_create(&rootdir, "newfile", &newfile)) {
        test_error("Fehler beim Anlegen der Datei");
    }

    ext2_inode_release(&rootdir);
    ext2_inode_update(&newfile);
    ext2_inode_release(&newfile);
    if (!ext2_fs_unmount(fs)) {
        test_interror("Unmounten fehlgeschlagen");
    }

    test_fsck();
    test_etool();

    return 0;
}
