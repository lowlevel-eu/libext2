#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test:  failed_mount
 * Autor: Antoine Kaufmann
 * Zweck: Prueft ob auch keine Superblockkopie mehr im fs ist, wenn das Mounten
 *        fehlschlaegt.
 */
int run_test(ext2_fs_t* fs)
{
    ext2_fs_mount(fs);

    if (fs->sb != NULL) {
        test_interror("Gueltige Superblockkopie in nicht gemountetem FS");
    }
    return 0;
}

