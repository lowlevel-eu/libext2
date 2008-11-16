#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test:  dir_find
 * Autor: Antoine Kaufmann
 * Zweck: ext2_dir_find testen
 */
int run_test(ext2_fs_t* fs)
{
    ext2_inode_t a;
    ext2_inode_t b;

    ext2_fs_mount(fs);

    if (!ext2_dir_find(fs, "/a/b/c", &a)) {
        test_interror("Suchen des Eintrages '/a/b/c' fehlgeschlagen");
    }

    if (!ext2_dir_find(fs, "a/b/c", &b)) {
        test_interror("Suchen des Eintrages 'a/b/c' fehlgeschlagen");
    }


    if (a.number != 14) {
        test_error("Inodenummer von '/a/b/c' stimmt nicht");
    }

    if (b.number != 14) {
        test_error("Inodenummer von '/a/b/c' stimmt nicht");
    }

    ext2_inode_release(&a);
    ext2_inode_release(&b);
    if (!ext2_fs_unmount(fs)) {
        test_interror("Unmounten fehlgeschlagen");
    }

    return 0;
}

