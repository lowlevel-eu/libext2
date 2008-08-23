#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test: fs_mount
 * Autor: Antoine Kaufmann
 * Zweck: ext2_fs_mount testen
 */
int run_test(ext2_fs_t* fs)
{
    if (!ext2_fs_mount(fs)) {
        test_error("Mounten fehlgeschlagen");
    }

    if (!fs->sb || !fs->cache_handle) {
        test_interror("Superblock oder Cache-handle nach dem Mounten == NULL");
    }

    if (!ext2_fs_unmount(fs)) {
        test_error("Unmounten fehlgeschlagen");
    }

    if (fs->sb || fs->cache_handle) {
        test_error("Superblock oder Cache-handle nach dem Unmounten != "
            "NULL");
    }

    return 0;
}
