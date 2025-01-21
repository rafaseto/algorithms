#include <iostream>
#include <vector>
#include <algorithm> 
#include <cmath>
using namespace std;

int randomPivotIndex(vector<int>& arr, int ini, int fim) {
    int size = fim - ini + 1; // calculando n
    return ini + abs(arr[ini]) % size;
}

int hoare(vector<int>&arr, int ini, int fim, int& num_trocas_chamadas) {
    int P_index = randomPivotIndex(arr, ini, fim);
    swap(arr[P_index], arr[ini]);
    num_trocas_chamadas++;
    
    int P = arr[ini]; // definindo o nosso pivo
    int i = ini - 1, j = fim + 1;

    while (1) {
        while (arr[--j] > P);
        while (arr[++i] < P);
        if (i < j) {
            num_trocas_chamadas++;
            swap(arr[i], arr[j]);   
        }
        else return j;
    }
}

void quicksort(vector<int>&arr, int ini, int fim, int& num_trocas_chamadas) {
    num_trocas_chamadas++;
    if (ini < fim) {
        int pivo = hoare(arr, ini, fim, num_trocas_chamadas);

        quicksort(arr, ini, pivo, num_trocas_chamadas);
        quicksort(arr, pivo + 1, fim, num_trocas_chamadas);
    }
}

int main() {
    vector<int> arr = {955, -32, 1, 9};

    int num_trocas_chamadas = 0;

    cout << "Original vector: ";
    for (int num : arr) {
        cout << num << " ";
    }
    cout << endl;

    quicksort(arr, 0 , arr.size() - 1, num_trocas_chamadas);

    cout << "Ordered vector: ";
    for (int num : arr) {
        cout << num << " ";
    }
    cout << endl;

    // Exibir as estatísticas
    cout << "Swaps and Calls: " << num_trocas_chamadas << endl;

    return 0;
}