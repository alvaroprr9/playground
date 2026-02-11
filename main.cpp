#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef int32_t i32;
typedef uint32_t u32;

typedef struct
{
    i32 *data;
    u32 capacity;
    u32 size;
    u32 initial_capacity;
} Stack;

Stack *stack_create(u32 initial_capacity)
{
    Stack *stack = (Stack *)malloc(sizeof(Stack));
    stack->data = (i32 *)malloc(initial_capacity * sizeof(i32));
    stack->capacity = initial_capacity;
    stack->size = 0;
    stack->initial_capacity = initial_capacity;
    return stack;
}

void stack_push(Stack *stack, i32 value)
{
    if (stack->size == stack->capacity)
    {
        u32 new_capacity = stack->capacity * 2;
        i32 *ndata = (i32 *)malloc(new_capacity * sizeof(i32));
        if (!ndata)
        {
            printf("Error redimensionando el stack\n");
            return;
        }

        for (u32 i = 0; i < stack->size; ++i)
            ndata[i] = stack->data[i];

        free(stack->data);
        stack->data = ndata;
        stack->capacity = new_capacity;

        printf("Capacidad aumentada a %u\n", new_capacity);
    }

    stack->data[stack->size] = value;
    stack->size++;
}

i32 stack_pop(Stack *stack)
{
    if (stack->size == 0)
    {
        printf("Error: stack vacio\n");
        return 0
    }

    stack->size--;
    i32 value = stack->data[stack->size];

    if (stack->size <= stack->capacity / 4 && stack->capacity / 2 >= stack->initial_capacity)
    {
        u32 new_capacity = stack->capacity / 2;
        i32 *ndata = (i32 *)malloc(new_capacity * sizeof(i32));
        if (!ndata)
        {
            printf("Error redimensionando el stack\n");
            return value;
        }

        for (u32 i = 0; i < stack->size; ++i)
            ndata[i] = stack->data[i];

        free(stack->data);
        stack->data = ndata;
        stack->capacity = new_capacity;

        printf("Capacidad reducida a %u\n", new_capacity);
    }

    return value;
}


void stack_free(Stack * stack)
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

    stack_push(stack, 10);

    printf("Pop: %d\n", stack_pop(stack));
    stack_free(stack);

    return 0;
}
