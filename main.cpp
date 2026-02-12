#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <memory>

typedef int32_t i32;
typedef uint32_t u32;

// 1 byte -> 8bits

typedef struct
{
    std::byte *data;
    u32 capacity;
    u32 size;
} Stack;

Stack *stack_create(u32 initial_capacity)
{
    Stack *stack = (Stack *)malloc(sizeof(Stack));
    stack->data = (std::byte *)malloc(initial_capacity);
    stack->capacity = initial_capacity;
    stack->size = 0;
    return stack;
}

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

void stack_push(Stack *stack, std::byte *memory, u32 size)
{
    if (stack->size + size > stack->capacity)
    {
        u32 new_capacity = stack->capacity * 2;
        std::byte *ndata = (std::byte *)malloc(new_capacity);
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

    std::byte *dst = stack->data + stack->size;
    std::byte *src = memory;

    memcpy(dst, src, size);

    stack->size += size;

    // stack->data[stack->size] = value;
    // stack->size++;
}

i32 stack_pop(Stack *stack)
{
}

void stack_free(Stack *stack)
{
    if (stack)
    {
        free(stack->data);
        free(stack);
    }
}

int main()
{
    Stack *stack = stack_create(2);

    if (!stack)
        return 1;

    // stack_push(stack, 10);

    printf("Pop: %d\n", stack_pop(stack));
    stack_free(stack);

    return 0;
}
