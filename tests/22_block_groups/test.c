#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test:  block_groups
 * Autor: Kevin Wolf
 * Zweck: Verschiedene Blockgruppen lesen. Der Testcode ist identisch mit Test
 *        16, das Image hat dieses Mal allerdings zwei Blockgruppen.
 */
int run_test(ext2_fs_t* fs)
{
    ext2_inode_t rootdir;
    ext2_inode_t testfile;
    ext2_dirent_t* dirent;
    char buf[4096];
    uint32_t pattern[1024];
    char* filename;
    int i;

    if (!ext2_fs_mount(fs)) {
        test_interror("Mounten fehlgeschlagen");
    }

    if (!ext2_inode_read(fs, 1, &rootdir)) {
        test_interror("Fehler beim Lesen des Rootverzeichnis-Inode");
    }

    dirent = ext2_dir_get(&rootdir, "nonexistent");
    if (dirent) {
        test_error("Datei /nonexistent wurde gefunden");
    }
    free(dirent);

    dirent = ext2_dir_get(&rootdir, "testfile");
    if (!dirent) {
        test_error("Datei /testfile wurde nicht gefunden");
    }

    filename = ext2_dir_alloc_name(dirent);
    if (strcmp(filename, "testfile")) {
        test_error("Eine falsche Datei wurde gefunden: %s", filename);
    }

    if (!ext2_inode_read(fs, dirent->inode - 1, &testfile)) {
        test_interror("Fehler beim Lesen des Testdatei-Inode");
    }

    for (i = 0; i < 1024; i++) {
        pattern[i] = 0x1337d00d;
    }

    for (i = 0; i < 64; i++) {
        if (!ext2_inode_readblk(&testfile, i, &buf)) {
            test_error("Block %d konnte nicht eingelesen werden", i);
        }

        if (memcmp(pattern, buf, ext2_sb_blocksize(fs->sb))) {
            test_error("Block %d entspricht nicht dem Muster", i);
        }
    }

    return 0;
}
