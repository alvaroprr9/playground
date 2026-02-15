#ifndef MY_STACK
#define MY_STACK

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

struct Stack;
Stack *stack_create(u32 initial_capacity, u32 elem_size);
void stack_push(Stack *stack, void *value);
void stack_pop(Stack *stack, void *out_value);
void stack_free(Stack *stack);

#endif
