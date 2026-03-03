 #include "stack_p.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <memory>

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t i8;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef float f32;
typedef double f64;

// 1 byte -> 8bits

struct Stack
{
    std::byte *data;
    u32 capacity;
    u32 size;
    u32 elem_size;
};

Stack *stack_create(u32 initial_capacity, u32 elem_size)
{
    Stack *stack = (Stack *)malloc(sizeof(Stack));
    stack->capacity = initial_capacity;
    stack->size = 0;
    stack->elem_size = elem_size;
    stack->data = (std::byte *)malloc(initial_capacity*initial_capacity);
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

void stack_resize(Stack *stack)
{
    u32 new_capacity = stack->capacity * 2;

    std::byte *ndata = (std::byte *)malloc(new_capacity * stack->elem_size);
    if (!ndata)
    {
        printf("Error redimensionando el stack\n");
        return;
    }

    memcpy(ndata, stack->data, stack->size * stack->elem_size);

    free(stack->data);

    stack->data = ndata;
    stack->capacity = new_capacity;
}

void stack_push(Stack *stack, void *element)
{
    if (stack->size == stack->capacity)
    {
        stack_resize(stack);
    }

    std::byte *dst = stack->data + (stack->size * stack->elem_size);

    memcpy(dst, element, stack->elem_size);

    stack->size++;
}

void stack_pop(Stack *stack, void *out_element)
{
    if (stack->size == 0)
    {
        printf("Stack underflow\n");
        return;
    }

    stack->size--;

    std::byte *src = stack->data + (stack->size * stack->elem_size);

    memcpy(out_element, src, stack->elem_size);
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
    Stack *intStack = stack_create(2, sizeof(i32));

    i32 a = 10;
    i32 b = 20;

    stack_push(intStack, &a);
    stack_push(intStack, &b);

    i32 result;
    stack_pop(intStack, &result);

    printf("Pop int: %d\n", result);

    stack_free(intStack);
}
