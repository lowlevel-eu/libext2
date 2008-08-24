#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test:  extend_indirect
 * Autor: Kevin Wolf
 * Zweck: Ueber das Ende einer Datei hinausschreiben, so dass indirekte
 *        Blocks angelegt werden muessen
 */
int run_test(ext2_fs_t* fs)
{
    ext2_inode_t rootdir;
    ext2_inode_t testfile;
    ext2_dirent_t* dirent;
    char buf[4096];
    uint32_t pattern[1024];
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

    if (!ext2_inode_read(fs, dirent->inode, &testfile)) {
        test_interror("Fehler beim Lesen des Testdatei-Inode");
    }
    free(dirent);

    for (i = 0; i < 1024; i++) {
        pattern[i] = 0xd00fc0de;
    }

    for (i = 0; i < 64; i++) {
        if (!ext2_inode_writeblk(&testfile, i, pattern)) {
            test_error("Block %d konnte nicht geschrieben werden", i);
        }
    }

    if (!ext2_inode_update(&testfile)) {
        test_interror("Fehler beim Schreiben des Testdatei-Inode");
    }

    ext2_fs_sync(fs);
    test_fsck();

    for (i = 0; i < 64; i++) {
        if (!ext2_inode_readblk(&testfile, i, buf, 1)) {
            test_error("Block %d konnte nicht eingelesen werden", i);
        }

        if (memcmp(pattern, buf, ext2_sb_blocksize(fs->sb))) {
            test_error("Block %d entspricht nicht dem Muster 0xd00fc0de", i);
        }
    }

    if (!ext2_fs_unmount(fs)) {
        test_interror("Unmounten fehlgeschlagen");
    }

    return 0;
}
