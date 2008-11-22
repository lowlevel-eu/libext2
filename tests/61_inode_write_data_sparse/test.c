#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test:  inode_write_data_sparse
 * Autor: Antoine Kaufmann
 * Zweck: Ein Sparse-File schreiben
 */
int run_test(ext2_fs_t* fs)
{
    ext2_inode_t rootdir;
    ext2_inode_t testfile;
    ext2_dirent_t* dirent;
    uint32_t pattern[1024];
    uint32_t offset;
    int i;

    if (!ext2_fs_mount(fs)) {
        test_interror("Mounten fehlgeschlagen");
    }

    if (!ext2_inode_read(fs, EXT2_ROOTDIR_INODE, &rootdir)) {
        test_interror("Fehler beim Lesen des Rootverzeichnis-Inode");
    }

    dirent = ext2_dir_get(&rootdir, "testfile");
    if (!dirent) {
        test_error("Datei /testfile wurde nicht gefunden");
    }
    ext2_inode_release(&rootdir);

    if (!ext2_inode_read(fs, dirent->inode, &testfile)) {
        test_interror("Fehler beim Lesen des Testdatei-Inode");
    }
    free(dirent);

    for (i = 0; i < 1024; i++) {
        pattern[i] = 0xd00fc0de;
    }

    offset = 12;
    for (i = 0; i < 1; i++) {
        if (!ext2_inode_writedata(&testfile, ext2_sb_blocksize(fs->sb)*(i +
            offset), ext2_sb_blocksize(fs->sb), pattern))
        {
            test_error("Block %d konnte nicht geschrieben werden", i);
        }
    }

    if (!ext2_inode_update(&testfile)) {
        test_interror("Fehler beim Schreiben des Testdatei-Inode");
    }

    ext2_fs_sync(fs);
    test_fsck();

    ext2_inode_release(&testfile);
    if (!ext2_fs_unmount(fs)) {
        test_interror("Unmounten fehlgeschlagen");
    }

    return 0;
}
