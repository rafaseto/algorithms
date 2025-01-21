#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <sstream>
using namespace std;

int randomPivotIndex(vector<int>& arr, int ini, int fim) {
    int size = fim - ini + 1; // calculando n
    return ini + abs(arr[ini]) % size;
}

int medianOfThree(vector<int>& arr, int ini, int fim) {
    int size = fim - ini + 1;
    int i = ini + size/4;
    int j = ini + size/2; 
    int k = ini + 3*size/4;

    int a = arr[i], b = arr[j], c = arr[k];

    if ((a < b && a > c) || (a > b && a < c)) return i;
    else if ((b < a && b > c) || (b > a && b < c)) return j;
    else return k;
}

int lomuto(vector<int>& arr, int ini, int fim, int& num_trocas_chamadas, string tipo) {
    int P_index;
    if (tipo == "random") {
        P_index = randomPivotIndex(arr, ini, fim);
        swap(arr[P_index], arr[fim]);
        num_trocas_chamadas++;
    } else if (tipo == "median") {
        P_index = medianOfThree(arr, ini, fim);
        swap(arr[P_index], arr[fim]);
        num_trocas_chamadas++;
    } else P_index = fim;

    int P = arr[fim], i = ini - 1;
    for (int j = ini; j < fim; j++) {
        if (arr[j] <= P) {
            swap(arr[++i], arr[j]);
            num_trocas_chamadas++;
        }
    }

    // A posicao certa do pivo eh a posicao i + 1
    swap(arr[++i], arr[fim]);
    num_trocas_chamadas++;

    return i;
}

int hoare(vector<int>&arr, int ini, int fim, int& num_trocas_chamadas, string tipo) {
    int P_index;
    
    if (tipo == "random") {
        P_index = randomPivotIndex(arr, ini, fim);
        swap(arr[P_index], arr[ini]);
        num_trocas_chamadas++;
    } else if (tipo == "median") {
        P_index = medianOfThree(arr, ini, fim);
        swap(arr[P_index], arr[ini]);
        num_trocas_chamadas++;
    } else P_index = ini;

    int P = arr[ini], i = ini - 1, j = fim + 1;

    while (1) {
        while (arr[--j] > P);
        while (arr[++i] < P);
        if (i < j) {
            swap(arr[i], arr[j]);
            num_trocas_chamadas++;   
        } else return j;
    }
}

void quicksort(vector<int>& arr, int ini, int fim, int& num_trocas_chamadas, string tipo, string metodo) {
    num_trocas_chamadas++;
    if (ini < fim) {
        int pivo;
        if (metodo == "lomuto") pivo = lomuto(arr, ini, fim, num_trocas_chamadas, tipo);
        else pivo = hoare(arr, ini, fim, num_trocas_chamadas, tipo);

        if (metodo == "lomuto") {
            quicksort(arr, ini, pivo - 1, num_trocas_chamadas, tipo, metodo);
            quicksort(arr, pivo + 1, fim, num_trocas_chamadas, tipo, metodo);
        } else {
            quicksort(arr, ini, pivo, num_trocas_chamadas, tipo, metodo);
            quicksort(arr, pivo + 1, fim, num_trocas_chamadas, tipo, metodo);
        }
    }
}

int main() {
    vector<int> arr = {834, 27, 39, 19, 3, -1, -33}; 

    int num_trocas_chamadas = 0;

    cout << "Original vector: ";
    for (int num : arr) {
        cout << num << " ";
    }
    cout << endl;

    quicksort(arr, 0, arr.size() - 1, num_trocas_chamadas, "padrao", "lomuto");

    cout << "Ordered vector: ";
    for (int num : arr) {
        cout << num << " ";
    }
    cout << endl;

    // Exibir as estatísticas
    cout << "Swaps and Calls: " << num_trocas_chamadas << endl;

    return 0;
}