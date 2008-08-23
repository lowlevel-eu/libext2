#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

static int handle_dirent(ext2_dirent_t* dirent, void* prv);
static int i = 0;

/**
 * Test:  remove_file
 * Autor: Kevin Wolf
 * Zweck: Einen Verzeichniseintrag samt Inode entfernen
 */
int run_test(ext2_fs_t* fs)
{
    ext2_inode_t rootdir;

    ext2_fs_mount(fs);

    if (!ext2_inode_read(fs, EXT2_ROOTDIR_INODE, &rootdir)) {
        test_interror("Inode konnte nicht eingelesen werden");
    }

    i = 0;
    ext2_dir_foreach(&rootdir, handle_dirent, NULL);
    if (i != 5) {
        test_error("Erwartet: 5 Dateien, gelesen %d", i);
    }

    if (!ext2_dir_unlink(&rootdir, "testfile")) {
        test_error("ext2_dir_unlink(testfile) gescheitert.");
    }
    if (!ext2_dir_unlink(&rootdir, "testlink")) {
        test_error("ext2_dir_unlink(testlink) gescheitert.");
    }

    test_fsck();
    test_etool();

    i = 0;
    ext2_dir_foreach(&rootdir, handle_dirent, NULL);
    if (i != 3) {
        test_error("Erwartet: 3 Dateien, gelesen %d", i);
    }

    if (!ext2_fs_unmount(fs)) {
        test_interror("Unmounten fehlgeschlagen");
    }

    return 0;
}

static int handle_dirent(ext2_dirent_t* dirent, void* prv)
{
    char* name = ext2_dir_alloc_name(dirent);
    char* expected;

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

        case 3:
            expected = "testlink";
            break;

        case 4:
            expected = "testfile";
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
