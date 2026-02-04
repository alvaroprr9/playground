#include <iostream>
using namespace std;

void helloWorld()
{
    cout << "HelloWorld" << endl;
}

void HelloWorldIf(int x)
{
    if (x == 3)
        helloWorld();
}

void HelloWorldMany(int n)
{
    for (int i = 0; i < n; i++)
        helloWorld();
}

void HelloWorldWhile()
{
    int i = 0;
    while (i < 4)
    {
        cout << "HelloWorld" << endl;
        i++;
    }
}

void imprimirArray(int *arr, int size)
{
    for (int i = 0; i < size; i++)
    {
        printf("%d\n", arr[i]);
    }
}

int main()
{

    int A[] = {1, 2, 3, 4};
    int B[] = {1, 2, 3, 4};
    constexpr int n = 4;
    int C[n];
    for (int i = 0; i < n; i++)
    {
        C[i] = A[i] + B[i];
    }
    for (int i = 0; i < n; i++)
    {
        printf("%d", C[i]);
    }
    return 0;
}
// int main()
// {
//     HelloWorldIf(3);
//     HelloWorldIf(2);
//     HelloWorldMany(4);
//     int n;
//     int n2 = 3;
//     int *ptr = &n;
//
//     cout << "cuantas veces?";
//     cin >> n;
//     HelloWorldMany(n);
//     int size;
//     cout << "Cuantos numeros quieres generar?";
//     cin >> size;
//     vector<int> numeros(size);
//
//     for (int i = 0; i < size; i++)
//     {
//         cout << "Ingresa el numero" << i + 1 << ":";
//         cin >> numeros[i];
//     }
//
//     cout << " Los numeros ingresados son: ";
//     for (int i = 0; i < size; i++)
//     {
//         cout << numeros[i] << " ";
//     }
//     cout << endl;
//     return 0;
// }
