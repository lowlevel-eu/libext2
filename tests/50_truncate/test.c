#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test:  truncate
 * Autor: Kevin Wolf
 * Zweck: Bestehende Datei kuerzen
 */
int run_test(ext2_fs_t* fs)
{
    ext2_inode_t rootdir, testfile;
    ext2_dirent_t* dirent;
    rootdir.raw.size = 0;

    ext2_fs_mount(fs);

    if (!ext2_inode_read(fs, EXT2_ROOTDIR_INODE, &rootdir)) {
        test_error("Inode konnte nicht eingelesen werden");
    }

    dirent = ext2_dir_get(&rootdir, "testfile");
    if (!dirent) {
        test_error("Datei /testfile wurde nicht gefunden");
    }

    if (!ext2_inode_read(fs, dirent->inode, &testfile)) {
        test_interror("Fehler beim Lesen des Testdatei-Inode");
    }

    if (testfile.raw.size != 65536) {
        test_error("Eingelesene Groesse (%d) stimmt nicht", rootdir.raw.size);
    }

    if (!ext2_inode_truncate(&testfile, 6000)) {
        test_error("Fehler in truncate");
    }

    if (testfile.raw.size != 6000) {
        test_error("Eingelesene Groesse (%d) stimmt nicht", rootdir.raw.size);
    }

    test_fsck();
    test_etool();

    return 0;
}
