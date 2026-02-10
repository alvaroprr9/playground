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
} Stack;
Stack *stack_create(u32 initial_capacity)
{
    Stack *stack = (Stack *)malloc(sizeof(Stack));
    stack->data = (i32 *)malloc(initial_capacity * sizeof(i32));
    stack->capacity = initial_capacity;
    stack->size = 0;
    return stack;
}

void stack_push(Stack *stack, i32 value)
{
    if (stack->size == stack->capacity)
    {
        u32 new_capacity = stack->capacity * 2;
        i32 *new_data = (i32 *)realloc(stack->data, new_capacity * sizeof(i32));

        if (!new_data)
        {
            printf("Error redimensionando el stack\n");
            return;
        }

        stack->data = new_data;
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
        printf("Error: stack vacío\n");
        return 0;
    }

    stack->size--;
    return stack->data[stack->size];
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

    stack_push(stack, 10);

    printf("Pop: %d\n", stack_pop(stack));
    stack_free(stack);

    return 0;
}
