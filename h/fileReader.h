#ifndef FILEREADER_H
#define FILEREADER_H

#include <stdio.h>

// Function to read a file and store lines in an array
FILE* read_file_to_lines(const char *filename, char ***lines, size_t **line_lengths, size_t *line_count);

#endif
