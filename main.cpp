#include "stack.hpp"

int main()
{
    // Stack de int
    Stack *int_stack = stack_create(2, sizeof(i32));

    f32 val;
    stack_push(int_stack, &val);

    i32 a = 10;
    i32 b = 20;

    stack_push(int_stack, &a);
    stack_push(int_stack, &b);

    stack_free(int_stack);

    // Stack de double
    Stack *double_stack = stack_create(2, sizeof(f64));
    stack_free(double_stack);
    return 0;
}
