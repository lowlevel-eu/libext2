#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test:  inode_read_data_sparse
 * Autor: Antoine Kaufmann
 * Zweck: inode_read_data testen mit Sparse File
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
        test_error("Datei /file wurde nicht gefunden");
    }

    if (!ext2_inode_read(fs, dirent->inode, &testfile)) {
        test_interror("Fehler beim Lesen des Testdatei-Inode");
    }
    free(dirent);

    size_t blksize = ext2_sb_blocksize(fs->sb);
    char buf[blksize];
    char cmp_buf[blksize];

    memset(buf, 0xFF, blksize);
    memset(cmp_buf, 0x0, blksize);

    ext2_inode_readdata(&testfile, 0, blksize, buf);
    if (memcmp(buf, cmp_buf, blksize)) {
        test_error("buf != cmp_buf");
    }

    if (!ext2_fs_unmount(fs)) {
        test_interror("Unmounten fehlgeschlagen");
    }

    return 0;
}

