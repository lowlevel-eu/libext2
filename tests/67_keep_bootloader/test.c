#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test: keep_bootloader
 * Autor: Kevin Wolf
 * Zweck: Beim Superblock-Update muss der Bootloader heil bleiben
 */
int run_test(ext2_fs_t* fs)
{
    ext2_superblock_t sb;
    char buf[1024], pattern[1024];

    memset(buf, 0, 1024);
    memset(pattern, 0x5a, 1024);

    ext2_fs_mount(fs);

    if (!ext2_sb_read(fs, &sb)) {
        test_error("Superblock konnte nicht eingelesen werden");
    }

    if (!ext2_sb_update(fs, &sb)) {
        test_error("Superblock konnte nicht geschrieben werden");
    }

    ext2_fs_sync(fs);

    if (!fs->dev_read(0, 1024, buf, fs->dev_private)) {
        test_error("Fehler beim Lesen vom Image");
    }

    if (memcmp(pattern, buf, 1024)) {
        test_error("Der Bootloader sieht kaputt aus");
    }

    return 0;
}
