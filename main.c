#include <stdio.h>
#include <stdlib.h>

// Describes one file within a .dat/.dir relationship
typedef struct {
    char filename[13]; // Maximum 12 characters and space for a null-terminator
    unsigned int filesize; // How many bytes to read
    unsigned int offset; // Where to read from in dat file
} FileEntry;

// Extracts a pair of .dat/.dir files
void datdir_extract(const char* dat_file, const char* dir_file, const char* output_dir) {
    FILE *dir_fp, *dat_fp, *out_fp;
    FileEntry entry;
    int file_count; // Number of files in .dir
    char output_path[256];

    // Open the .dir file for reading
    dir_fp = fopen(dir_file, "rb");
    if (!dir_fp) {
        perror("Error opening .dir file");
        return;
    }

    // Read the number of files
    fread(&file_count, sizeof(int), 1, dir_fp);

    // Open the .dat file for reading
    dat_fp = fopen(dat_file, "rb");
    if (!dat_fp) {
        perror("Error opening .dat file");
        fclose(dir_fp);
        return;
    }

    // Free memory and close files
    fclose(dir_fp);
    fclose(dat_fp);
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Usage: %s <dat_file> <dir_file> <output_dir>\n", argv[0]);
        return 1;
    }
    printf("Extracting %s...\n", argv[1]);
    datdir_extract(argv[1], argv[2], argv[3]);
}