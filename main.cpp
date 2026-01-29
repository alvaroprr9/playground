#include <iostream>
#include <vector>
using namespace std;

void helloWorld()
{
    cout << "HelloWorld" << endl;
}

void HelloWorldIf(int x)
{
    if (x == 3)
    {
        helloWorld();
    }
}

void HelloWorldMany(int n)
{
    for (int i = 0; i < n; i++)
    {
        helloWorld();
    }
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


void imprimirArray(int *arr, int size){
    for (int i = 0; i < size; i++)
    {
        printf("%d", *(arr + i)); 
    }
    printf("\n");
}


int main()
{
    HelloWorldIf(3);
    HelloWorldIf(2);
    HelloWorldMany(4);
    int n;
    cout << "cuantas veces?";
    cin >> n;
    HelloWorldMany(n);
    int tamaño;
    cout << "Cuantos numeros quieres generar?";
    cin >> tamaño;
    vector<int> numeros(tamaño);

    for (int i = 0; i < tamaño; i++){
        cout << "Ingresa el numero" << i + 1 << ":";
        cin >> numeros[i];
    }

    cout << " Los números ingresados son: ";
    for (int i = 0; i < tamaño; i++){
        cout << numeros[i] << " ";
    }
    cout << endl;
    return 0;
}

