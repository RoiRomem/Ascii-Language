#include "../h/loopHandler.h"
#include <stdlib.h>

#define LOOP_STACK_GROWTH_FACTOR 2

LoopStack* init_loop_stack(size_t initial_capacity) {
    LoopStack *stack = malloc(sizeof(LoopStack));
    if (!stack) return NULL;
    
    stack->positions = malloc(initial_capacity * sizeof(size_t));
    if (!stack->positions) {
        free(stack);
        return NULL;
    }
    
    stack->capacity = initial_capacity;
    stack->size = 0;
    return stack;
}

bool push_loop(LoopStack *stack, size_t position) {
    if (stack->size >= stack->capacity) {
        size_t new_capacity = stack->capacity * LOOP_STACK_GROWTH_FACTOR;
        size_t *new_positions = realloc(stack->positions, new_capacity * sizeof(size_t));
        if (!new_positions) return false;
        
        stack->positions = new_positions;
        stack->capacity = new_capacity;
    }
    
    stack->positions[stack->size++] = position;
    return true;
}

bool pop_loop(LoopStack *stack, size_t *position) {
    if (stack->size == 0) return false;
    
    *position = stack->positions[--stack->size];
    return true;
}

bool peek_loop(const LoopStack *stack, size_t *position) {
    if (stack->size == 0) return false;
    
    *position = stack->positions[stack->size - 1];
    return true;
}

void free_loop_stack(LoopStack *stack) {
    if (stack) {
        free(stack->positions);
        free(stack);
    }
}