#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

static int handle_dirent(ext2_dirent_t* dirent, void* prv);

/**
 * Test:  dir_entries
 * Autor: Kevin Wolf
 * Zweck: Eintraege des Rootverzeichnisses einlesen
 */
int run_test(ext2_fs_t* fs)
{
    ext2_inode_t rootdir;

    ext2_fs_mount(fs);

    if (!ext2_inode_read(fs, EXT2_ROOTDIR_INODE, &rootdir)) {
        test_interror("Inode konnte nicht eingelesen werden");
    }

    ext2_dir_foreach(&rootdir, handle_dirent, NULL);

    ext2_inode_release(&rootdir);
    if (!ext2_fs_unmount(fs)) {
        test_interror("Unmounten fehlgeschlagen");
    }

    return 0;
}

static int handle_dirent(ext2_dirent_t* dirent, void* prv)
{
    char* name = ext2_dir_alloc_name(dirent);
    char* expected;
    static int i = 0;

    switch (i++) {
        case 0:
            expected = ".";
            break;

        case 1:
            expected = "..";
            break;

        case 2:
            expected = "lost+found";
            break;

        default:
            test_error("Eintrag zu viel: %s", name);
    }

    if (strcmp(name, expected)) {
        test_error("Erwartet: %s; gelesen: %s\n", expected, name);
    }

    free(name);

    return 0;
}
