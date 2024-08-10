#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// Describes one file within a .dat/.dir relationship
#pragma pack(push, 1) // Ensure there is no padding in the struct
typedef struct {
    char filename[12]; // Maximum 11 characters and space for a null-terminator
    uint32_t filesize; // How many bytes to read
    uint32_t offset; // Where to read from in dat file
} FileEntry;
#pragma pack(pop)

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
    fread(&file_count, sizeof(uint32_t), 1, dir_fp);

    // Open the .dat file for reading
    dat_fp = fopen(dat_file, "rb");
    if (!dat_fp) {
        perror("Error opening .dat file");
        fclose(dir_fp);
        return;
    }

    // Extract each file in the index
    for (int i = 0; i < file_count; i++) {
        fread(&entry, sizeof(FileEntry), 1, dir_fp);

        // Filenames lack null terminator so we add one for display
        char filename[13];
        strncpy(filename, entry.filename, 12);
        filename[12] = '\0'; // Manually null-terminate

        printf("Name: %s, Offset %d, Size: %d\n", filename, entry.offset, entry.filesize);
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