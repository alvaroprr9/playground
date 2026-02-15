#include "stack_g.hpp"
#include <stdio.h>

// + + + + - - - - - - - -
// >
// memory -> + + + + + + | |
//
// size = 4
// psize = 6
//
// dst = data + size
// src = memory
// s = psize
//
// memcpy(dst, src, s)
//
// data = > +
//
//
// size = 4
//
// memory, 4
//
//
//

struct Stack
{
    void *data;
    u32 capacity;
    u32 size;
    u32 elem_size;
};

Stack *stack_create(u32 initial_capacity, u32 elem_size)
{
    Stack *stack = (Stack *)malloc(sizeof(Stack));
    stack->data = (std::byte *)malloc(initial_capacity * elem_size);
    stack->capacity = initial_capacity;
    stack->elem_size = elem_size;
    stack->size = 0;
    return stack;
}

void stack_push(Stack *stack, void *value)
{
    if (stack->size == stack->capacity)
    {
        u32 new_capacity = stack->capacity * 2;
        void *ndata = realloc(stack->data, new_capacity * stack->elem_size);
        if (!ndata)
        {
            printf("Error redimensionando el stack\n");
            return;
        }

        memcpy(ndata, stack->data, stack->size);

        free(stack->data);
        stack->data = ndata;
        stack->capacity = new_capacity;

        printf("Capacidad aumentada a %u\n", new_capacity);
    }

    void *dest = (std::byte *)stack->data + (stack->size * stack->elem_size);

    memcpy(dest, value, stack->elem_size);

    stack->size++;
}
void stack_pop(Stack *stack, void *out_value)
{
    if (stack->size == 0)
    {
        printf("Stack vacio\n");
        return;
    }

    stack->size--;
    void *src = (std::byte *)stack->data + (stack->size * stack->elem_size);
    memcpy(out_value, src, stack->elem_size);
}
void stack_free(Stack *stack)
{
    if (stack)
    {
        free(stack->data);
        free(stack);
    }
}
