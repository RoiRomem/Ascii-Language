#ifndef LOOP_HANDLER_H
#define LOOP_HANDLER_H

#include <stddef.h>
#include <stdbool.h>

// Structure to store loop information
typedef struct {
    size_t *positions;     // Array of loop start positions
    size_t capacity;       // Current capacity of the stack
    size_t size;          // Current number of loops
} LoopStack;

// Initialize loop stack
LoopStack* init_loop_stack(size_t initial_capacity);

// Push loop position onto stack
bool push_loop(LoopStack *stack, size_t position);

// Pop loop position from stack
bool pop_loop(LoopStack *stack, size_t *position);

// Peek at top loop position
bool peek_loop(const LoopStack *stack, size_t *position);

// Free loop stack
void free_loop_stack(LoopStack *stack);

#endif
