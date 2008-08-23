#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test:  inode_type
 * Autor: Antoine Kaufmann
 * Zweck: Typen von Inodes pruefen
 */
int run_test(ext2_fs_t* fs)
{
    ext2_inode_t file;
    ext2_inode_t dir;
    ext2_inode_t symlink;

    ext2_fs_mount(fs);

    if (!ext2_inode_read(fs, 12, &file)) {
        test_interror("Datei-Inode konnte nicht eingelesen werden");
    }

    if (!ext2_inode_read(fs, 11, &dir)) {
        test_interror("Verzeichnis-Inode konnte nicht eingelesen werden");
    }

    if (!ext2_inode_read(fs, 13, &symlink)) {
        test_interror("Symlink-Inode konnte nicht eingelesen werden");
    }


    if (!EXT2_INODE_IS_FILE(&file)) {
        test_error("Datei-Inode nicht als solcher erkannt");
    }

    if (!EXT2_INODE_IS_DIR(&dir)) {
        test_error("Verzeichnis-Inode nicht als solcher erkannt");
    }

    if (!EXT2_INODE_IS_SYMLINK(&symlink)) {
        test_error("Symlink-Inode nicht als solcher erkannt");
    }

    if (!ext2_fs_unmount(fs)) {
        test_interror("Unmounten fehlgeschlagen");
    }

    return 0;
}

