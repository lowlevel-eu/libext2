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

    // Neuen Inode alloziern
    if (!ext2_inode_alloc(fs, &newdir)) {
        test_error("Fehler beim allozieren des Inodes");
    }

    // Verzeichniseintrag anlegen
    if (!ext2_dir_link(&rootdir, &newdir, "newdir")) {
        test_error("Fehler beim anlegen des Verzeichniseintrages");
    }

    // Doteintraege anlegen
    if (!ext2_dir_link(&newdir, &newdir, ".")) {
        test_error("Fehler beim anlegen des Dot-Eintrages");
    }
    if (!ext2_dir_link(&newdir, &rootdir, "..")) {
        test_error("Fehler beim anlegen des Root-Eintrages");
    }


    if (!ext2_inode_update(&rootdir)) {
        test_interror("Fehler beim Aktualisieren des Rootverzeichnis-Inode");
    }

    newdir.raw.deletion_time = 0;
    newdir.raw.mode = EXT2_INODE_MODE_DIR;
    if (!ext2_inode_update(&newdir)) {
        test_interror("Fehler beim Aktualisieren des neuen Inodes");
    }

    // Verzeichnisanzahl in der Blockgruppe erhoehen
    ext2_blockgroup_t bg;
    ext2_bg_read(fs, 0, &bg);
    bg.used_directories++;
    ext2_bg_update(fs, 0, &bg);


    test_fsck();

    return 0;
}
