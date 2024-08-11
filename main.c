#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

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
        buffer = (uint8_t*)malloc(entry.filesize);
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

int datdir_filter(const struct dirent *name)
{
  return 1;
}

int roundUp(int numToRound, int multiple)
{
    if (multiple == 0)
        return numToRound;

    int remainder = numToRound % multiple;
    if (remainder == 0)
        return numToRound;

    return numToRound + multiple - remainder;
}

void datdir_repack(const char* dat_folder, const char* dat_file, const char* dir_file) {
    FILE *dir_fp, *dat_fp, *file_fp;
    char output_path[256];

    /* Reconstruct the .dir index */
    DIR *d;
    struct dirent **namelist;

    uint32_t file_count = 0; // Have to count real files, since by default scandir includes folders, symlinks etc.

    uint32_t n = scandir(dat_folder, &namelist, datdir_filter, alphasort);
    if (n == -1) {
        perror("scandir");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n; i++) {
        // Ensure it's a real file, not symlink or folder
        if (namelist[i]->d_type == DT_REG) {
            file_count++;
            printf("NAME: %s RECORD LENGTH: %d \n", namelist[i]->d_name, namelist[i]->d_reclen);
        }
    }

    // Open the new .dir file
    printf("Dir_file: %s\n", dir_file);
    dir_fp = fopen(dir_file, "wb");
    if (!dir_fp) {
        perror("Error opening .dir file");
        return;
    }

    // First write the bytes for the file count
    fwrite(&file_count, sizeof(uint32_t), 1, dir_fp);

    uint32_t file_size = 0;
    uint32_t offset = -; // We calculate the running offset by adding the file sizes together

    for (int i = 0; i < n; i++) {
        // Ensure it's a real file, not symlink or folder
        if (namelist[i]->d_type == DT_REG) {
            // Read file and write correct bytes to dir file
            snprintf(output_path, sizeof(output_path), "%s/%s", dat_folder, namelist[i]->d_name);

            file_fp = fopen(output_path, "rb");
            
            fseek(file_fp, 0L, SEEK_END); // Skip to end to get size
            file_size = ftell(file_fp);
            fseek(file_fp, 0L, SEEK_SET); // Skip to start again

            // Write index contents (name, size, offset)
            fwrite(&namelist[i]->d_name, sizeof(char), 12, dir_fp); // Need to pad file name later to not rely on zeroed data
            fwrite(&file_size, sizeof(uint32_t), 1, dir_fp);
            fwrite(&offset, sizeof(uint32_t), 1, dir_fp);

            offset += roundUp(file_size, 2048);

            fclose(file_fp);
        }
    }

    fclose(dir_fp);

    /* Repack the .dat file using the generated index */
    // Open the new .dat file
    printf("Dat_file: %s\n", dat_file);
    dat_fp = fopen(dat_file, "wb");
    if (!dat_fp) {
        perror("Error opening .dat file");
        return;
    }

    // Create a buffer to read files into
    uint8_t buffer[1024] = {0}; // Allocate 256MB of space for files
    uint32_t read_size = 0;
    uint32_t padding_size = 0; // Need to pad to nearest 2kb on each file

    for (int i = 0; i < n; i++) {
        // Ensure it's a real file, not symlink or folder
        if (namelist[i]->d_type == DT_REG) {
            file_size = 0;
            printf("PACKING %s into .dat\n", namelist[i]->d_name);
            // Clear the buffer so it's zeroed for reading
            memset(buffer, 0, sizeof buffer);

            // Read file and write data with padding to dat file
            snprintf(output_path, sizeof(output_path), "%s/%s", dat_folder, namelist[i]->d_name);

            file_fp = fopen(output_path, "rb");
            if (!file_fp) {
                perror("Error opening .dat file");
                return;
            }
            
            while ((read_size = fread(buffer, 1, sizeof(buffer), file_fp)) > 0)
            {
                fwrite(&buffer, sizeof(uint8_t), read_size, dat_fp);
                file_size += read_size;
            }

            // Write padding to nearest 2kb
            padding_size = roundUp(file_size, 2048) - file_size;
            uint8_t* zeros = calloc(padding_size, sizeof(uint8_t));
            fwrite(zeros, sizeof(uint8_t), padding_size, dat_fp);
            free(zeros);

            fclose(file_fp);
        }
    }

    fclose(dat_fp);
}

typedef enum FileMode {
    EXTRACT,
    PACK
} FileMode;

void print_usage(char *argv[]) {
    const char* help_string = "Usage: %s [-ep] <args>\n\n"
                              "Example commands:\n"
                              "%s -e potter.dat potter.dir out #Extract .dat/.dir pair\n"
                              "%s -p out potter.dat potter.dir #Repack extracted .dat/.dir pair files\n"
                              "";
    fprintf(stderr, help_string, argv[0], argv[0], argv[0]);
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
            // hagrid -p <path> <filename_out>
            printf("Pack!\n");
            datdir_repack(argv[optind], argv[optind+1], argv[optind+2]);
            break;
        default:
            print_usage(argv);
            exit(EXIT_FAILURE);
    }
}