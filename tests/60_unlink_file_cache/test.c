#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test:  unlink_file_cache
 * Autor: Antoine Kaufmann
 * Zweck: ext2_dir_unlink hat bis jetzt dev_read direkt benutzt um die
 *        Indirekten Blocks freizugeben. Das geht aber mit Caching schief,
 *        wie dieser Test zeigt.
 */
int run_test(ext2_fs_t* fs)
{
    ext2_inode_t rootdir;
    ext2_inode_t newfile;

    ext2_fs_mount(fs);

    if (!ext2_inode_read(fs, EXT2_ROOTDIR_INODE, &rootdir)) {
        test_interror("Inode konnte nicht eingelesen werden");
    }

    if (!ext2_file_create(&rootdir, "newfile", &newfile)) {
        test_error("Fehler beim Anlegen der Datei");
    }

    char buffer[1024];
    int i;
    memset(buffer, 0xaa, 1024);
    for (i = 0; i < 64; i++) {
        ext2_inode_writedata(&newfile, i * 1024, 1024, buffer);
    }

    if (!ext2_dir_unlink(&rootdir, "newfile")) {
        test_error("ext2_dir_unlink(testfile) gescheitert.");
    }
    ext2_inode_update(&newfile);
    ext2_inode_update(&rootdir);

    ext2_fs_sync(fs);
    test_fsck();
    test_etool();

    ext2_inode_release(&rootdir);
    ext2_inode_release(&newfile);
    if (!ext2_fs_unmount(fs)) {
        test_interror("Unmounten fehlgeschlagen");
    }

    return 0;
}

