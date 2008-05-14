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

    if (!ext2_inode_read(fs, 1, &rootdir)) {
        test_interror("Fehler beim Lesen des Rootverzeichnis-Inode");
    }

    // Neuen Inode alloziern
    if (!ext2_inode_alloc(fs, &newfile)) {
        test_error("Fehler beim allozieren des Inodes");
    }

    // Verzeichniseintrag anlegen
    if (!ext2_dir_link(&rootdir, &newfile, "newfile")) {
        test_error("Fehler beim anlegen des Verzeichniseintrages");
    }

    if (!ext2_inode_update(&rootdir)) {
        test_interror("Fehler beim Aktualisieren des Rootverzeichnis-Inode");
    }

    newfile.raw.deletion_time = 0;
    newfile.raw.mode = EXT2_INODE_MODE_FILE;
    if (!ext2_inode_update(&newfile)) {
        test_interror("Fehler beim Aktualisieren des neuen Inodes");
    }

    test_fsck();

    return 0;
}
