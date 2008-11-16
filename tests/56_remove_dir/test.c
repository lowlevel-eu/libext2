#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test:  remove_file
 * Autor: Antoine Kaufmann
 * Zweck: Einen Verzeichniseintrag samt Inode entfernen
 */
int run_test(ext2_fs_t* fs)
{
    ext2_inode_t rootdir;
    ext2_inode_t testdir;
    ext2_dirent_t* dirent;

    ext2_fs_mount(fs);

    if (!ext2_inode_read(fs, EXT2_ROOTDIR_INODE, &rootdir)) {
        test_interror("Inode konnte nicht eingelesen werden");
    }

    if (!(dirent = ext2_dir_get(&rootdir, "testdir"))) {
        test_interror("Verzeichniseintrag testdir nicht gefunden");
    }

    if (!ext2_inode_read(fs, dirent->inode, &testdir)) {
        test_interror("tesdir-Inode konnte nicht geladen werden");
    }
    free(dirent);

    if (!ext2_dir_unlink(&testdir, "..") || !ext2_dir_unlink(&testdir, ".")) {
        test_interror("Loeschen des . oder .. Eintrages im Verzeichnis "
            "fehlgeschlagen");
    }

    if (!ext2_dir_unlink(&rootdir, "testdir")) {
        test_error("ext2_dir_unlink(testdir) gescheitert.");
    }
    ext2_inode_release(&rootdir);

    ext2_fs_sync(fs);
    test_fsck();
    test_etool();

    ext2_inode_release(&testdir);
    if (!ext2_fs_unmount(fs)) {
        test_interror("Unmounten fehlgeschlagen");
    }

    return 0;
}

