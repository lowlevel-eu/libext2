#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test:  readblk_cont
 * Autor: Antoine Kaufmann
 * Zweck: Aneinanderhaengedne Bloecke einlesen
 */
int run_test(ext2_fs_t* fs)
{
    ext2_inode_t rootdir;
    ext2_inode_t testfile;
    ext2_dirent_t* dirent;
    char buf[1024 * 10];
    uint32_t pattern[1024];
    char* filename;
    int i;

    ext2_fs_mount(fs);

    ext2_superblock_t sb;
    if (!ext2_sb_read(fs, &sb)) {
        test_interror("Fehler beim Lesen des Superblocks");
    }

    if (!ext2_inode_read(fs, EXT2_ROOTDIR_INODE, &rootdir)) {
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
    free(filename);

    if (!ext2_inode_read(fs, dirent->inode, &testfile)) {
        test_interror("Fehler beim Lesen des Testdatei-Inode");
    }
    free(dirent);

    for (i = 0; i < 1024; i++) {
        pattern[i] = 0x1337d00d;
    }

    if (ext2_inode_readblk(&testfile, 0, &buf, 10) != 10) {
        test_error("Blocks konnten nicht eingelesen werden", i);
    }
    for (i = 0; i < 10; i++) {
        if (memcmp(pattern, buf + (i * 1024), ext2_sb_blocksize(&sb))) {
            test_error("Block %d entspricht nicht dem Muster", i);
        }
    }

    return 0;
}
