#include <ext2.h>
#include <string.h>
#include <stdio.h>
#include <test.h>

/**
 * Test: dev_read
 * Autor: Antoine Kaufmann
 * Zweck: dev_read in fs-Struktur testen
 */
int run_test(ext2_fs_t* fs)
{
    char buffer[10];
    const char* test = "deadbeef\n";
    size_t test_len = strlen(test);
    int i;

    buffer[9] = 0;
    for (i = 0; i < 50; i++) {
        if (!fs->dev_read(i * test_len, test_len, buffer)) {
            test_error("Fehler beim Einlesen im %d. Durchlauf", i);
        }

        if (strcmp(test, buffer) != 0) {
            test_error("Fehler beim Vergleich im %d. Durchlauf", i);
        }
    }
    return 0;
}
