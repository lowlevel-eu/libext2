#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test:  writeblk_file_size
 * Autor: Kevin Wolf
 * Zweck: Dateigroesse nach ext2_inode_writeblk testen
 */
int run_test(ext2_fs_t* fs)
{
    ext2_inode_t rootdir;
    ext2_inode_t testfile;
    ext2_dirent_t* dirent;
    unsigned char pattern[4096];
    uint32_t bs;

    if (!ext2_fs_mount(fs)) {
        test_interror("Mounten fehlgeschlagen");
    }

    bs = ext2_sb_blocksize(fs->sb);
    if (bs != 1024) {
        test_error("Falsche Blockgroesse");
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

    memset(pattern, 0xaa, 4096);
    ext2_inode_writeblk(&testfile, 1, &pattern);

    if (testfile.raw->size != 1024 + bs) {
        test_error("Dateigroesse != 1024 + bs");
    }

    ext2_inode_writeblk(&testfile, 0, &pattern);

    if (testfile.raw->size != 1024 + bs) {
        test_error("Dateigroesse != 1024 + bs [2]");
    }

    ext2_inode_update(&testfile);

    ext2_inode_release(&testfile);
    if (!ext2_fs_unmount(fs)) {
        test_interror("Unmounten fehlgeschlagen");
    }

    test_fsck();
    test_etool();

    return 0;
}
