#include <stdio.h>

void sumArrays(int *a, int *b, int *c, int n)
{
    for (int i = 0; i < n; i++)
    {
        c[i] = a[i] + b[i];
    }
}

int main()
{
    int A[] = {1, 2, 3, 4};
    int B[] = {1, 2, 3, 4};
    constexpr int n = 4;
    int C[n];
    sumArrays(A, B, C, n);
    for (int i = 0; i < 4; i++)
    {
        printf("%d ", C[i]);
    }
    return 0;
}
