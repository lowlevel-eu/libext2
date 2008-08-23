#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

static int handle_dirent(ext2_dirent_t* dirent, void* prv);

static char* files[1024];
static int num_files = 0;

/**
 * Test:  remove_file_multibg
 * Autor: Kevin Wolf
 * Zweck: Dateien loeschen (mehrere Blockgruppen und mit indirekten Blocks)
 */
int run_test(ext2_fs_t* fs)
{
    ext2_inode_t rootdir;
    int i;

    if (!ext2_fs_mount(fs)) {
        test_interror("Image konnte nicht gemountet werden");
    }

    if (!ext2_inode_read(fs, EXT2_ROOTDIR_INODE, &rootdir)) {
        test_interror("Inode konnte nicht eingelesen werden");
    }

    ext2_dir_foreach(&rootdir, handle_dirent, NULL);

    for (i = 0; i < num_files; i++) {

        if (!ext2_dir_unlink(&rootdir, files[i])) {
            test_error("ext2_dir_unlink(%s) gescheitert.", files[i]);
        }
        free(files[i]);

        test_fsck();
        test_etool();
    }

    return 0;
}

static int handle_dirent(ext2_dirent_t* dirent, void* prv)
{
    char* name = ext2_dir_alloc_name(dirent);
    if (strncmp("test", name, 4) == 0) {
        files[num_files++] = name;
    } else {
        free(name);
    }
    return 0;
}
