#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test:  read_elf_magic
 * Autor: Antoine Kaufmann
 * Zweck: ELF-Magic von LD einlesen
 */
int run_test(ext2_fs_t* fs)
{
    ext2_inode_t inode;

    ext2_fs_mount(fs);

    if (!ext2_inode_read(fs, 12, &inode)) {
        test_interror("Inode konnte nicht eingelesen werden");
    }

    char blkbuf[inode.raw.size];
    unsigned int* magic = (unsigned int*) blkbuf;
    ext2_inode_readdata(&inode, 0, inode.raw.size, blkbuf);
    if (*magic != 0x464C457F) {
        test_error("Eingelesene Magic Number stimmt nicht: 0x%x", *magic);
    }

    return 0;
}
