#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/syslimits.h>

void copy_file(const char *source_path, const char *dest_path) {
    int src_fd = open(source_path, O_RDONLY);
    if (src_fd < 0) {
        perror("Error opening source file");
        exit(EXIT_FAILURE);
    }

    int dest_fd = open(dest_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dest_fd < 0) {
        perror("Error opening/creating destination file");
        close(src_fd);
        exit(EXIT_FAILURE);
    }

    char buffer[4096];
    ssize_t bytes_read;
    while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
        if (write(dest_fd, buffer, bytes_read) != bytes_read) {
            perror("Error writing to destination file");
            close(src_fd);
            close(dest_fd);
            exit(EXIT_FAILURE);
        }
    }

    if (bytes_read < 0) {
        perror("Error reading source file");
    }

    close(src_fd);
    close(dest_fd);
}

void copy_to_directory(const char *source_path, const char *dest_dir) {
    char *source_basename = basename(strdup(source_path));
    char dest_path[PATH_MAX];
    snprintf(dest_path, sizeof(dest_path), "%s/%s", dest_dir, source_basename);

    copy_file(source_path, dest_path);
}

int main(int argc, char *argv[]) {
    // ensure program has at least two arguments; one source file and a destination
    if (argc < 3) {
        fprintf(stderr, "Usage: %s source_file destination_file\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    struct stat dest_stat;
    char *dest_path = argv[argc - 1]; // last argument is the destination

    // check if destination exists
    if (stat(dest_path, &dest_stat) == 0) {
        if (S_ISDIR(dest_stat.st_mode)) {
            // destination is a directory
            printf("Destination is a directory.\n");
            for (int i = 1; i < argc - 1; i++) {
                char *source_basename = basename(argv[i]); // get file name from source path
                char full_dest_path[PATH_MAX]; // full destination path, directory + source file name 
                snprintf(full_dest_path, sizeof(full_dest_path), "%s/%s", dest_path, source_basename);
                copy_file(argv[i], full_dest_path); // copy each source file to destination directory
            }
        } else if (S_ISREG(dest_stat.st_mode)) {
            // destination is existing regular file
            printf("Destination is a file.\n");
            // overwrite destination file with content of the first source file 
            copy_file(argv[1], dest_path);
        } else {
            // destination exists but is neither a file nor a directory
            fprintf(stderr, "Error: Destination exists but is neither a file nor a directory.");
            exit(EXIT_FAILURE);
        }
    } else {
        // destination does not exist, handle different cases
        printf("Destination does not exist.\n");
        if (argc == 3) {
            // if only one source file, treat destination as a file to be created
            printf("Creating destination file.\n");
            copy_file(argv[1], dest_path); // create and copy source file to new destination 
        } else {
            // multiple source files but destination does not exist
            fprintf(stderr, "Error: Destination directory does not exist.\n");
            exit(EXIT_FAILURE);
        }
    }
}
