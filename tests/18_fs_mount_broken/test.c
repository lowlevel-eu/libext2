#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test: fs_mount_broken
 * Autor: Antoine Kaufmann
 * Zweck: ext2_fs_mount mit fehlerhaftem Image testen
 */
int run_test(ext2_fs_t* fs)
{
    if (ext2_fs_mount(fs)) {
        test_error("Mounten war erfolgreich, trotz fehlerhaftem Image");
    }

    return 0;
}
