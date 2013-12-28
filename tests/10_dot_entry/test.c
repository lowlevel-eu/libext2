#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test:  dir_entries
 * Autor: Kevin Wolf
 * Zweck: Eintraege des Rootverzeichnisses einlesen
 */
int run_test(ext2_fs_t* fs)
{
    ext2_inode_t rootdir;
    ext2_dirent_t *dirent;
    char buf[4096];

    ext2_fs_mount(fs);

    if (!ext2_inode_read(fs, EXT2_ROOTDIR_INODE, &rootdir)) {
        test_interror("Inode konnte nicht eingelesen werden");
    }

    if (ext2_inode_readblk(&rootdir, 0, &buf, 1) != 1) {
        test_interror("Block konnte nicht eingelesen werden");
    }

    dirent = (ext2_dirent_t*) buf;
    if (memcmp(dirent->name, ".", 2)) {
        test_error("Verzeichniseintrag . erwartet");
    }

    if (dirent->inode != 2) {
        test_error("Verzeichniseintrag fuer . verweist auf falsche Inode",
            dirent->inode);
    }

    ext2_inode_release(&rootdir);
    ext2_fs_unmount(fs);

    return 0;
}
