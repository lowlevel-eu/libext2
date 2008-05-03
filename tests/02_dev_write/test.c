#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test: dev_write
 * Autor: Antoine Kaufmann
 * Zweck: dev_write in fs-Struktur testen
 */
int run_test(ext2_fs_t* fs)
{
    char buffer[256];
    int i;
    int j;

    // Testdaten schreiben
    for (j = 0; j < 1024; j++) {
        for (i = 0; i < 256; i++) {
            buffer[i] = j + i;
        }

        if (!fs->dev_write(j * 256, 256, buffer, fs->dev_private)) {
            test_error("Fehler beim Schreiben im %d. Durchlauf", j);
        }
    }

    // Geschriebene Daten pruefen
    for (j = 0; j < 1024; j++) {
        if (!fs->dev_read(j * 256, 256, buffer, fs->dev_private)) {
            test_error("Fehler beim Einlesen im %d. Durchlauf", j);
        }

        for (i = 0; i < 256; i++) {
            if (buffer[i] != (char) (j + i)) {
                test_error("Fehler beim Pruefen im %d. Durchlauf, Byte %d",
                    j, i);
            }
        }
    }
    return 0;
}
