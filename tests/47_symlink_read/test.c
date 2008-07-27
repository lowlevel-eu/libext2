#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test:  symlink_read
 * Autor: Kevin Wolf
 * Zweck: Symlink einlesen
 */
int run_test(ext2_fs_t* fs)
{
    char* symlink;
    ext2_inode_t rootdir, testfile;
    ext2_dirent_t* dirent;

    ext2_fs_mount(fs);

    if (!ext2_inode_read(fs, EXT2_ROOTDIR_INODE, &rootdir)) {
        test_error("Inode konnte nicht eingelesen werden");
    }

    dirent = ext2_dir_get(&rootdir, "symlink");
    if (!dirent) {
        test_error("Datei /symlink wurde nicht gefunden");
    }

    if (!ext2_inode_read(fs, dirent->inode, &testfile)) {
        test_interror("Fehler beim Lesen des Testdatei-Inode");
    }

    if (!(symlink = ext2_symlink_read(&testfile))) {
        test_error("Konnte Symlink nicht einlesen");
    }

    if (strcmp(symlink, "/mnt/lost+found/")) {
        test_error("Symlink-Ziel erwartet: '%s', gelesen: '%s'",
            "/mnt/lost+found/", symlink);
    }

    return 0;
}
