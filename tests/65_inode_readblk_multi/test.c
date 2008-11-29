#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>
#include <stdint.h>

/**
 * Test:  inode_readblk_multi
 * Autor: Kevin Wolf
 * Zweck: inode_readblk mit mehreren Bloecken am Stueck und
 *        ueberdimensioniertem Lesepuffer testen
 *
 * Das Image enthaelt im Hauptverzeichnis eine Datei testfile, die
 * aus 2048 Bytes mit dem Muster 'test' besteht. Der Test liest
 * 4096 Bytes ein, so dass zwei Bloecke sparse sind.
 */
int run_test(ext2_fs_t* fs)
{
    ext2_inode_t rootdir;
    ext2_inode_t testfile;
    ext2_dirent_t* dirent;

    uint32_t pattern[2048 / 4];
    uint32_t buf[4096 / 4];
    int i;

    ext2_fs_mount(fs);

    if (!ext2_inode_read(fs, EXT2_ROOTDIR_INODE, &rootdir)) {
        test_interror("Inode konnte nicht eingelesen werden");
    }

    dirent = ext2_dir_get(&rootdir, "testfile");
    if (!dirent) {
        test_interror("Datei /testfile wurde nicht gefunden");
    }

    if (!ext2_inode_read(fs, dirent->inode, &testfile)) {
        test_interror("Inode der Testdatei konnte nicht eingelesen werden");
    }

    if (ext2_inode_readblk(&testfile, 0, buf, 4) != 4) {
        test_interror("Konnte nicht aus der Testdatei lesen");
    }

    for (i = 0; i < 2048 / 4; i++) {
        pattern[i] = 0x74736574; // 'test'
    }
    if (memcmp(buf, pattern, 2048)) {
        test_error("Fehler beim Einlesen der eigentlichen Datei.");
    }

    memset(pattern, 0, 2048);
    if (memcmp(buf + (2048 / 4), pattern, 2048)) {
        test_error("Fehler beim Einlesen der Sparse-Bloecke.");
    }

    ext2_inode_release(&rootdir);
    if (!ext2_fs_unmount(fs)) {
        test_interror("Unmounten fehlgeschlagen");
    }

    return 0;
}
