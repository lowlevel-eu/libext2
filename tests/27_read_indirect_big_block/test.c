#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test:  read_indirect_big_blocks
 * Autor: Antoine Kaufmann
 * Zweck: Aus einer grossen Datei lesen, so dass indirekte Bloecke benutzt
 *        werden muessen und das mit einem Dateisystem mit Bloecken von 2048
 *        Bytes
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

    ext2_fs_mount(fs);

    ext2_superblock_t sb;
    if (!ext2_sb_read(fs, &sb)) {
        test_interror("Fehler beim Lesen des Superblocks");
    }

    if (!ext2_inode_read(fs, 1, &rootdir)) {
        test_interror("Fehler beim Lesen des Rootverzeichnis-Inode");
    }

    dirent = ext2_dir_get(fs, &rootdir, "nonexistent");
    if (dirent) {
        test_error("Datei /nonexistent wurde gefunden");
    }
    free(dirent);

    dirent = ext2_dir_get(fs, &rootdir, "testfile");
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

    for (i = 0; i < 460; i++) {
        if (!ext2_inode_readblk(fs, &testfile, i, &buf)) {
            test_error("Block %d konnte nicht eingelesen werden", i);
        }

        if (memcmp(pattern, buf, ext2_sb_blocksize(&sb))) {
            test_error("Block %d entspricht nicht dem Muster", i);
        }
    }

    return 0;
}
