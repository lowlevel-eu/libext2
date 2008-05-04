#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>
#include <signal.h>
#include <setjmp.h>

static void handle_segfault(int sig);
jmp_buf env;

/**
 * Test:  failed_mount
 * Autor: Antoine Kaufmann
 * Zweck: Prueft ob auch keine Superblockkopie mehr im fs ist, wenn das Mounten
 *        fehlschlaegt.
 */
int run_test(ext2_fs_t* fs)
{
    ext2_fs_mount(fs);

    signal(SIGSEGV, handle_segfault);
    if (setjmp(env)) {
        return 0;
    }
    fs->sb->block_count++;

    test_interror("Gueltige Superblockkopie in nicht gemountetem FS");
    return 0;
}

static void handle_segfault(int sig)
{
    longjmp(env, 1);
}

