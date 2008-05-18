#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test:  inode_type
 * Autor: Antoine Kaufmann
 * Zweck: Typen von Inodes ohne Makros pruefen
 */
int run_test(ext2_fs_t* fs)
{
    ext2_inode_t file;
    ext2_inode_t dir;
    ext2_inode_t symlink;

    ext2_fs_mount(fs);

    if (!ext2_inode_read(fs, 11, &file)) {
        test_interror("Datei-Inode konnte nicht eingelesen werden");
    }

    if (!ext2_inode_read(fs, 10, &dir)) {
        test_interror("Verzeichnis-Inode konnte nicht eingelesen werden");
    }

    if (!ext2_inode_read(fs, 12, &symlink)) {
        test_interror("Symlink-Inode konnte nicht eingelesen werden");
    }


    if (ext2_inode_type(&file) != EXT2_IT_FILE) {
        test_error("Datei-Inode nicht als solcher erkannt");
    }

    if (ext2_inode_type(&dir) != EXT2_IT_DIR) {
        test_error("Verzeichnis-Inode nicht als solcher erkannt");
    }

    if (ext2_inode_type(&symlink) != EXT2_IT_SYMLINK) {
        test_error("Symlink-Inode nicht als solcher erkannt");
    }

    return 0;
}

