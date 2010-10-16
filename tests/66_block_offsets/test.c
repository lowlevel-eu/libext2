#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

static int cmp_offsets_readdata(ext2_fs_t* fs, ext2_inode_t* inode,
    uint32_t start, size_t count, size_t spcount)
{
    size_t blksize = ext2_sb_blocksize(fs->sb);
    char buf[blksize * count];
    char cmp_buf[blksize * count];
    uint64_t offsets[count + spcount];
    int i;

    memset(buf, 0xFF, blksize);
    memset(cmp_buf, 0x0, blksize);

    if (!ext2_inode_blockoffsets(inode, start, count + spcount, offsets)) {
        test_error("Holen der Blockoffsets fehlgeschlagen");
    }

    for (i = 0; i < count + spcount; i++) {
        if ((i < count) && (offsets[i] == EXT2_OFF_SPARSE)) {
            printf("%d\n", start);
            test_error("Block unerwartet als nicht alloziert markiert");
        } else if ((i > count) && (offsets[i] != EXT2_OFF_SPARSE)) {
            test_error("Block unerwartet als alloziert markiert");
        }
    }

    for (i = 0; i < count; i++) {
        if (!fs->dev_read(offsets[i], blksize, buf + blksize*i,
            fs->dev_private))
        {
            test_interror("Lesefehler (buf)");
        }
    }

    if (!ext2_inode_readblk(inode, start, cmp_buf, count)) {
        test_interror("Lesefehler (cmp_buf)");
    }

    return memcmp(buf, cmp_buf, blksize * count);
}

/**
 * Test:  ext2_inode_blockoffsets
 * Autor: Antoine Kaufmann
 * Zweck: ext2_inode_block_offsets testen
 */
int run_test(ext2_fs_t* fs)
{
    ext2_inode_t rootdir;
    ext2_inode_t testfile;
    ext2_dirent_t* dirent;

    ext2_fs_mount(fs);

    if (!ext2_inode_read(fs, EXT2_ROOTDIR_INODE, &rootdir)) {
        test_interror("Inode konnte nicht eingelesen werden");
    }

    dirent = ext2_dir_get(&rootdir, "file");
    if (!dirent) {
        test_error("Datei /file wurde nicht gefunden");
    }
    ext2_inode_release(&rootdir);

    if (!ext2_inode_read(fs, dirent->inode, &testfile)) {
        test_interror("Fehler beim Lesen des Testdatei-Inode");
    }
    free(dirent);

    if (cmp_offsets_readdata(fs, &testfile, 0, 2, 6)) {
        test_error("Fehler bei nur direkt");
    }
    if (cmp_offsets_readdata(fs, &testfile, 11, 2, 6)) {
        test_error("Fehler bei direkt/indirekt");
    }
    if (cmp_offsets_readdata(fs, &testfile, 12+255, 2, 6)) {
        test_error("Fehler bei indirekt/doppelt indirekt");
    }
    if (cmp_offsets_readdata(fs, &testfile, 12+256+256*256-1, 2, 6)) {
        test_error("Fehler bei doppelt indirekt/dreifach indirekt");
    }
    if (cmp_offsets_readdata(fs, &testfile,
        12+256+256*256+256*256*256-2, 2, 0))
    {
        test_error("Fehler bei nur dreifach indirekt");
    }

    return 0;
}

