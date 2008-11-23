#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test:  truncate_sparsefile
 * Autor: Antoine Kaufmann
 * Zweck: Ein Sparsefile verkuerzen
 */
int run_test(ext2_fs_t* fs)
{
    ext2_inode_t rootdir;
    ext2_inode_t testfile;
    ext2_dirent_t* dirent;

    ext2_fs_mount(fs);

    if (!ext2_inode_read(fs, EXT2_ROOTDIR_INODE, &rootdir)) {
        test_interror("Inode konnte nicht eingelesen werden");
    }

    dirent = ext2_dir_get(&rootdir, "file");
    if (!dirent) {
        test_interror("Datei /testfile wurde nicht gefunden");
    }
    ext2_inode_release(&rootdir);

    if (!ext2_inode_read(fs, dirent->inode, &testfile)) {
        test_interror("Inode der Testdatei konnte nicht eingelesen werden");
    }

    if (!ext2_inode_truncate(&testfile, 0)) {
        test_error("Truncate fehlgeschlagen");
    }
    ext2_inode_release(&testfile);

    ext2_fs_unmount(fs);
    test_fsck();

    return 0;
}

