#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test:  remove_fast_symlink
 * Autor: Antoine Kaufmann
 * Zweck: Einen Verzeichniseintrag samt Inode entfernen
 */
int run_test(ext2_fs_t* fs)
{
    ext2_inode_t rootdir;

    ext2_fs_mount(fs);

    if (!ext2_inode_read(fs, EXT2_ROOTDIR_INODE, &rootdir)) {
        test_interror("Inode konnte nicht eingelesen werden");
    }

    if (!ext2_dir_unlink(&rootdir, "fastsymlink")) {
        test_error("ext2_dir_unlink(fastsymlink) gescheitert.");
    }
    ext2_inode_release(&rootdir);

    ext2_fs_sync(fs);
    test_fsck();
    test_etool();

    if (!ext2_fs_unmount(fs)) {
        test_interror("Unmounten fehlgeschlagen");
    }

    return 0;
}

