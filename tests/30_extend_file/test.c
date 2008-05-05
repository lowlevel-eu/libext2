#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test:  extend_file
 * Autor: Kevin Wolf
 * Zweck: Ueber das Ende einer Datei hinausschreiben
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

    if (!ext2_inode_read(fs, 1, &rootdir)) {
        test_interror("Fehler beim Lesen des Rootverzeichnis-Inode");
    }

    dirent = ext2_dir_get(fs, &rootdir, "testfile");
    if (!dirent) {
        test_error("Datei /testfile wurde nicht gefunden");
    }

    if (!ext2_inode_read(fs, dirent->inode - 1, &testfile)) {
        test_interror("Fehler beim Lesen des Testdatei-Inode");
    }

    for (i = 0; i < 1024; i++) {
        pattern[i] = 0xd00fc0de;
    }

    for (i = 3; i < 7; i++) {
        if (!ext2_inode_writeblk(fs, &testfile, i, &buf)) {
            test_error("Block %d konnte nicht geschrieben werden", i);
        }
    }

    test_fsck();

    for (i = 3; i < 7; i++) {
        if (!ext2_inode_readblk(fs, &testfile, i, &pattern)) {
            test_error("Block %d konnte nicht eingelesen werden", i);
        }

        if (memcmp(pattern, buf, ext2_sb_blocksize(fs->sb))) {
            test_error("Block %d entspricht nicht dem Muster 0xd00fc0de", i);
        }
    }

    return 0;
}
