#include <stdio.h>

// Describes one file within a .dat/.dir relationship
typedef struct {
    char filename[13]; // Maximum 12 characters and space for a null-terminator
    unsigned int filesize; // How many bytes to read
    unsigned int offset; // Where to read from in dat file
} FileEntry;

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Usage: %s <dat_file> <dir_file> <output_dir>\n", argv[0]);
        return 1;
    }
    printf("Extracting %s...\n", argv[1]);
}