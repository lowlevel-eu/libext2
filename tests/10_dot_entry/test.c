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
    char buf[4096];

    ext2_fs_mount(fs);

    if (!ext2_inode_read(fs, 1, &rootdir)) {
        test_interror("Inode konnte nicht eingelesen werden");
    }

    if (!ext2_inode_readblk(&rootdir, 0, &buf)) {
        test_interror("Block konnte nicht eingelesen werden");
    }

    if (*((uint16_t*)((ext2_dirent_t*) buf)->name) != 0x002e) {
        test_error("Verzeichniseintrag . erwartet");
    }

    if (((ext2_dirent_t*) buf)->inode != 2) {
        test_error("Verzeichniseintrag fuer . verweist auf falsche Inode",
            ((ext2_dirent_t*) buf)->inode);
    }

    return 0;
}
