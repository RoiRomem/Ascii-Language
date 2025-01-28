#include "../h/ArrayManager.h"
#include <stdlib.h>
#include <stdio.h>

// Function to increase the size of the array
void IncreaseArraySize(unsigned char **arr, size_t current_size) {
    // Reallocate memory to increase the size of the array
    *arr = realloc(*arr, (current_size + 1) * sizeof(unsigned char));
    if (*arr == NULL) {
        perror("Error reallocating memory");
        exit(EXIT_FAILURE); // Exit if memory allocation fails
    }
    // Initialize the new cell to 0
    (*arr)[current_size] = 0;
}