#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>


static void check_buffer(ext2_inode_t* testfile, int a, int b)
{
    int i;
    unsigned char buf[4096];

    ext2_inode_readblk(testfile, 0, &buf[0]);
    ext2_inode_readblk(testfile, 1, &buf[1024]);

    for (i = 0; i < 2048; i++) {
        if ((i < a) && (buf[i] != 0xaa)) {
            test_error("Erwartet 0xaa an %d: a = %d, b = %d\n", i, a, b);
        } else if ((i >= a) && (i < b) && (buf[i] != 0xbb)) {
            test_error("Erwartet 0xaa an %d: a = %d, b = %d\n", i, a, b);
        } else if ((i >= b) && (buf[i] != 0xcc)) {
            test_error("Erwartet 0xaa an %d: a = %d, b = %d\n", i, a, b);
        }
    }
}

/**
 * Test:  inode_write_data
 * Autor: Kevin Wolf
 * Zweck: inode_write_data testen
 */
int run_test(ext2_fs_t* fs)
{
    ext2_inode_t rootdir;
    ext2_inode_t testfile;
    ext2_dirent_t* dirent;
    unsigned char pattern[4096];

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

    memset(pattern, 0xaa, 4096);
    ext2_inode_writeblk(&testfile, 0, &pattern);
    ext2_inode_writedata(&testfile, 1024, 1024, &pattern);
    check_buffer(&testfile, 2048, 2048);

    memset(pattern, 0xcc, 4096);
    ext2_inode_writedata(&testfile, 2048 - 42, 42, &pattern);
    check_buffer(&testfile, 2048 - 42, 2048 - 42);

    memset(pattern, 0xbb, 4096);
    ext2_inode_writedata(&testfile, 1024 - 64, 64 + 1024 - 30, &pattern);
    check_buffer(&testfile, 1024 - 64, 2048 - 30);

    memset(pattern, 0xbb, 4096);
    ext2_inode_writedata(&testfile, 0, 1024 - 64, &pattern);
    check_buffer(&testfile, 0, 2048 - 30);

    memset(pattern, 0xaa, 4096);
    ext2_inode_writedata(&testfile, 0, 2048, &pattern);
    check_buffer(&testfile, 2048, 2048);

    return 0;
}
