#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "h/fileReader.h"
#include "h/constants.h"
#include "h/ArrayManager.h"
#include "h/loopHandler.h"

#define INITIAL_LOOP_CAPACITY 16

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Initialize variables
    char **lines = NULL;
    size_t *line_lengths = NULL;
    size_t line_count = 0;
    size_t arraySize = STARTING_SIZE;

    // Initialize byte array
    unsigned char *byteListArray = calloc(arraySize, sizeof(unsigned char));
    unsigned char *currentPosition = byteListArray;  // Initialize currentPosition to the start of byteListArray
    if (!byteListArray) {
        perror("Error allocating memory for byteListArray");
        return EXIT_FAILURE;
    }

    // Initialize loop stack
    LoopStack *loops = init_loop_stack(INITIAL_LOOP_CAPACITY);
    if (!loops) {
        free(byteListArray);
        perror("Error initializing loop stack");
        return EXIT_FAILURE;
    }

    // Read file
    FILE *file = read_file_to_lines(argv[1], &lines, &line_lengths, &line_count);
    if (!file) {
        free_loop_stack(loops);
        free(byteListArray);
        return EXIT_FAILURE;
    }

    // Process each line
    for (size_t i = 0; i < line_count; i++) {
        for (size_t j = 0; j < line_lengths[i] && lines[i][j] != '\0'; j++) {
            unsigned char current_char = (unsigned char)lines[i][j];

            if (isspace(current_char)) continue;
            if (current_char == COMMENT) break;  // Stop processing at comment

            switch (current_char) {
                case STARTLOOP: // Start of loop
                    if (*currentPosition == 0) {
                        // Skip to matching ENDLOOP
                        int nested = 1;
                        while (nested > 0 && ++j < line_lengths[i]) {
                            if (lines[i][j] == STARTLOOP) nested++;
                            else if (lines[i][j] == ENDLOOP) nested--;
                        }
                        if (nested > 0) {
                            fprintf(stderr, "Error: Unmatched %c at line %zu\n", ENDLOOP, i + 1);
                            goto cleanup;
                        }
                    } else {
                        // Push current position to loop stack
                        if (!push_loop(loops, j)) {
                            fprintf(stderr, "Error: Loop stack overflow\n");
                            goto cleanup;
                        }
                    }
                    break;

                case ENDLOOP: // End of loop
                    {
                        size_t loop_start;
                        if (!pop_loop(loops, &loop_start)) {
                            fprintf(stderr, "Error: Unmatched %c at line %zu\n", ENDLOOP, i + 1);
                            goto cleanup;
                        }
                        if (*currentPosition != 0) {
                            j = loop_start - 1; // Will be incremented in for loop
                        }
                    }
                    break;

                case NEXTCELL:
                    // Check array bounds before incrementing
                    if ((size_t)(currentPosition - byteListArray) + 1 >= arraySize) {
                        IncreaseArraySize(&byteListArray, arraySize);
                        arraySize *= 2; // Double the size (or choose another strategy)
                    }
                    currentPosition++;
                    break;

                case PREVCELL:
                    // Prevent going below the first cell
                    if (currentPosition > byteListArray) {
                        currentPosition--;
                    }
                    break;

                case INCRESE:
                    (*currentPosition)++;
                    break;

                case DECRESE:
                    (*currentPosition)--;
                    break;

                case OUTPUT:
                    // Output all the values up to currentPosition
                    printf("[");
                    for (unsigned char *ptr = byteListArray; ptr < currentPosition; ptr++) {
                        if (ptr != byteListArray) printf(", ");
                        printf("%c", *ptr);
                    }
                    printf("]\n");
                    break;

                case INPUT:
                    *currentPosition = getchar();
                    break;

                default:
                    if (!isspace(current_char)) {
                        fprintf(stderr, "Syntax error at line %zu, position %zu: '%c'\n",
                                i + 1, j + 1, current_char);
                        goto cleanup;
                    }
            }
        }
    }

cleanup:
    // Clean up resources
    if (lines) {
        for (size_t i = 0; i < line_count; i++) {
            free(lines[i]);
        }
        free(lines);
    }
    free(line_lengths);
    free(byteListArray);
    free_loop_stack(loops);
    if (file) fclose(file);

    return 0;
}
