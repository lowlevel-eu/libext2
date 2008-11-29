#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>


/**
 * Test:  link_unlink_tests
 * Autor: Antoine Kaufmann
 * Zweck: Dateien in der selben Reihenfolge erstellen und loeschen wie vim es
 *        tut. Das fuehrt naemlich bis jetzt zu einem kaputten Dateisystem.
 */
int run_test(ext2_fs_t* fs)
{
    ext2_inode_t rootdir;
    ext2_inode_t afile;
    ext2_inode_t bfile;

    if (!ext2_fs_mount(fs)) {
        test_interror("Mounten fehlgeschlagen");
    }

    if (!ext2_inode_read(fs, EXT2_ROOTDIR_INODE, &rootdir)) {
        test_interror("Fehler beim Lesen des Rootverzeichnis-Inode");
    }

    if (!ext2_file_create(&rootdir, "afile", &afile)) {
        test_error("Fehler beim Anlegen der Datei a");
    }

    if (!ext2_file_create(&rootdir, "bfile", &bfile)) {
        test_error("Fehler beim Anlegen der Datei b");
    }

    ext2_inode_release(&bfile);
    if (!ext2_dir_unlink(&rootdir, "bfile")) {
        test_error("Fehler beim Loeschen der Datei a");
    }

    ext2_inode_release(&afile);
    if (!ext2_dir_unlink(&rootdir, "afile")) {
        test_error("Fehler beim Loeschen der Datei a");
    }

    if (!ext2_file_create(&rootdir, "afile", &afile)) {
        test_error("Fehler beim Anlegen der Datei a");
    }

    if (!ext2_file_create(&rootdir, "bfile", &bfile)) {
        test_error("Fehler beim Anlegen der Datei b");
    }

    ext2_inode_release(&bfile);
    if (!ext2_dir_unlink(&rootdir, "bfile")) {
        test_error("Fehler beim Loeschen der Datei a");
    }

    ext2_inode_release(&afile);
    ext2_inode_release(&rootdir);
    if (!ext2_fs_unmount(fs)) {
        test_interror("Unmounten fehlgeschlagen");
    }

    test_fsck();
    test_etool();

    return 0;
}
