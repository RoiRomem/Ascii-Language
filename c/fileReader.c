#include "../h/fileReader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define INITIAL_BUFFER_SIZE 1024
#define BUFFER_GROWTH_FACTOR 2

// Helper function to check if the file has a ".ascc" extension
int has_ascc_extension(const char *filename) {
    const char *ext = ".ascc";
    size_t len = strlen(filename);
    size_t ext_len = strlen(ext);

    // Check if the file ends with ".ascc"
    if (len < ext_len) return 0;
    return strcmp(filename + len - ext_len, ext) == 0;
}

FILE* read_file_to_lines(const char *filename, char ***lines, size_t **line_lengths, size_t *line_count) {
    // Check if the file has a ".ascc" extension
    if (!has_ascc_extension(filename)) {
        fprintf(stderr, "Error: File must have a '.ascc' extension\n");
        return NULL;
    }

    FILE *file;
    char *buffer = NULL;
    size_t buffer_size = INITIAL_BUFFER_SIZE;

    // Initialize output parameters
    *lines = NULL;
    *line_lengths = NULL;
    *line_count = 0;

    // Open the file
    file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    // Allocate initial buffer
    buffer = malloc(buffer_size);
    if (buffer == NULL) {
        perror("Error allocating initial buffer");
        fclose(file);
        return NULL;
    }

    // Read the file line by line
    char *current_pos = buffer;
    size_t remaining_size = buffer_size;
    int c;

    while ((c = fgetc(file)) != EOF) {
        // Check if we need to grow the buffer
        if (remaining_size <= 2) { // Leave room for newline and null terminator
            size_t current_offset = current_pos - buffer;
            size_t new_size = buffer_size * BUFFER_GROWTH_FACTOR;

            char *new_buffer = realloc(buffer, new_size);
            if (new_buffer == NULL) {
                perror("Error reallocating buffer");
                free(buffer);
                fclose(file);
                return NULL;
            }

            buffer = new_buffer;
            current_pos = buffer + current_offset;
            buffer_size = new_size;
            remaining_size = buffer_size - current_offset;
        }

        // Add character to buffer
        *current_pos++ = (char)c;
        remaining_size--;

        // Check for end of line
        if (c == '\n' || remaining_size <= 1) {
            // Null terminate the current line
            *current_pos = '\0';
            size_t line_length = current_pos - buffer;

            // Reallocate the lines array
            char **new_lines = realloc(*lines, (*line_count + 1) * sizeof(char*));
            size_t *new_lengths = realloc(*line_lengths, (*line_count + 1) * sizeof(size_t));

            if (new_lines == NULL || new_lengths == NULL) {
                perror("Error reallocating arrays");
                free(buffer);
                free(*lines);
                free(*line_lengths);
                fclose(file);
                return NULL;
            }

            *lines = new_lines;
            *line_lengths = new_lengths;

            // Allocate and copy the line
            (*lines)[*line_count] = malloc(line_length + 1);
            if ((*lines)[*line_count] == NULL) {
                perror("Error allocating memory for line");
                free(buffer);
                // Free previously allocated lines
                for (size_t i = 0; i < *line_count; i++) {
                    free((*lines)[i]);
                }
                free(*lines);
                free(*line_lengths);
                fclose(file);
                return NULL;
            }

            memcpy((*lines)[*line_count], buffer, line_length + 1);
            (*line_lengths)[*line_count] = line_length;
            (*line_count)++;

            // Reset buffer position
            current_pos = buffer;
            remaining_size = buffer_size;
        }
    }

    // Handle last line if it doesn't end with a newline
    if (current_pos > buffer) {
        *current_pos = '\0';
        size_t line_length = current_pos - buffer;

        // Reallocate arrays for the last line
        char **new_lines = realloc(*lines, (*line_count + 1) * sizeof(char*));
        size_t *new_lengths = realloc(*line_lengths, (*line_count + 1) * sizeof(size_t));

        if (new_lines == NULL || new_lengths == NULL) {
            perror("Error reallocating arrays for last line");
            free(buffer);
            // Free previously allocated lines
            for (size_t i = 0; i < *line_count; i++) {
                free((*lines)[i]);
            }
            free(*lines);
            free(*line_lengths);
            fclose(file);
            return NULL;
        }

        *lines = new_lines;
        *line_lengths = new_lengths;

        // Allocate and copy the last line
        (*lines)[*line_count] = malloc(line_length + 1);
        if ((*lines)[*line_count] == NULL) {
            perror("Error allocating memory for last line");
            free(buffer);
            // Free previously allocated lines
            for (size_t i = 0; i < *line_count; i++) {
                free((*lines)[i]);
            }
            free(*lines);
            free(*line_lengths);
            fclose(file);
            return NULL;
        }

        memcpy((*lines)[*line_count], buffer, line_length + 1);
        (*line_lengths)[*line_count] = line_length;
        (*line_count)++;
    }

    // Clean up temporary buffer
    free(buffer);

    return file;
}
