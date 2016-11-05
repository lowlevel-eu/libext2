#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test: duplicate_entries
 * Autor: Kevin Wolf
 * Zweck: Zwei Sachen mit gleichem Namen im gleichen Verzeichnis gehen nicht
 */
int run_test(ext2_fs_t* fs)
{
    ext2_inode_t rootdir;
    ext2_inode_t new;
    ext2_inode_t failing;

    if (!ext2_fs_mount(fs)) {
        test_interror("Mounten fehlgeschlagen");
    }

    if (!ext2_inode_read(fs, EXT2_ROOTDIR_INODE, &rootdir)) {
        test_interror("Fehler beim Lesen des Rootverzeichnis-Inode");
    }

    /* Verzeichnis anlegen und versuchen, etwas gleichnamiges zu erstellen */
    if (!ext2_dir_create(&rootdir, "dir", &new)) {
        test_error("Fehler beim Anlegen des ersten Verzeichnisses 'dir'");
    }

    if (ext2_dir_create(&rootdir, "dir", &failing)) {
        test_error("Kein Fehler beim doppelten Verzeichnis 'dir'");
    }
    if (ext2_file_create(&rootdir, "dir", &failing)) {
        test_error("Kein Fehler bei der doppelten Datei 'dir'");
    }
    if (ext2_symlink_create(&rootdir, "dir", "x", &failing)) {
        test_error("Kein Fehler beim doppelten Symlink 'dir'");
    }
    if (ext2_dir_link(&rootdir, &new, "dir")) {
        test_error("Kein Fehler beim doppelten Hardlink 'dir'");
    }
    ext2_inode_release(&new);


    /* Datei anlegen und versuchen, etwas gleichnamiges zu erstellen */
    if (!ext2_dir_create(&rootdir, "file", &new)) {
        test_error("Fehler beim Anlegen des ersten Verzeichnisses 'file'");
    }

    if (ext2_dir_create(&rootdir, "file", &failing)) {
        test_error("Kein Fehler beim doppelten Verzeichnis 'file'");
    }
    if (ext2_file_create(&rootdir, "file", &failing)) {
        test_error("Kein Fehler bei der doppelten Datei 'file'");
    }
    if (ext2_symlink_create(&rootdir, "file", "x", &failing)) {
        test_error("Kein Fehler beim doppelten Symlink 'file'");
    }
    if (ext2_dir_link(&rootdir, &new, "file")) {
        test_error("Kein Fehler beim doppelten Hardlink 'file'");
    }
    ext2_inode_release(&new);


    ext2_inode_release(&rootdir);
    ext2_fs_sync(fs);
    test_etool();
    test_fsck();

    return 0;
}
