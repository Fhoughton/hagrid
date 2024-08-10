#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

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
    uint8_t *buffer; // File data buffer

    for (int i = 0; i < file_count; i++) {
        fread(&entry, sizeof(FileEntry), 1, dir_fp);

        // Filenames lack null terminator so we add one for display
        char filename[13];
        strncpy(filename, entry.filename, 12);
        filename[12] = '\0'; // Manually null-terminate

        // Allocate buffer for file data
        buffer = (char*)malloc(entry.filesize);
        if (!buffer) {
            perror("Memory allocation failed");
            fclose(dir_fp);
            fclose(dat_fp);
            return;
        }

        // Read the file data from the .dat file
        fseek(dat_fp, entry.offset, SEEK_SET);
        fread(buffer, entry.filesize, 1, dat_fp);

        // Resolve the output file path
        snprintf(output_path, sizeof(output_path), "%s/%s", output_dir, filename);

        // Write the extracted file to the output directory
        out_fp = fopen(output_path, "wb");
        if (out_fp) {
            fwrite(buffer, entry.filesize, 1, out_fp);
            fclose(out_fp);
            printf("Extracted %s (size: %u, offset: %u) to %s\n", filename, entry.filesize, entry.offset, output_path);
        } else {
            perror("Error creating output file");
        }

        // Free the buffer
        free(buffer);
    }

    // Free memory and close files
    fclose(dir_fp);
    fclose(dat_fp);
}

typedef enum FileMode {
    EXTRACT,
    PACK
} FileMode;

void print_usage(char *argv[]) {
    fprintf(stderr, "Usage: %s [-ep] [dat_file] [dir_file] [output_dir]\n", argv[0]);
}

int main(int argc, char *argv[]) {
    FileMode file_mode = EXTRACT;
    int opt;

    while ((opt = getopt(argc, argv, "ep")) != -1) {
        switch (opt) {
        case 'e': file_mode = EXTRACT; break;
        case 'p': file_mode = PACK; break;
        default:
            print_usage(argv);
            exit(EXIT_FAILURE);
        }
    }

    // Check enough arguments were given
    switch (file_mode) {
        case EXTRACT:
            if ((argc - optind) < 3) {
                print_usage(argv);
                exit(EXIT_FAILURE);
            }
            datdir_extract(argv[optind], argv[optind+1], argv[optind+2]);
            break;
        case PACK:
            printf("Pack!\n");
            break;
        default:
            print_usage(argv);
            exit(EXIT_FAILURE);
    }
}