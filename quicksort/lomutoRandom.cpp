#include <iostream>
#include <vector>
#include <algorithm> 
using namespace std;

int randomPivotIndex(vector<int>& arr, int ini, int fim) {
    int size = fim - ini + 1; // calculando n
    return ini + abs(arr[ini]) % size;
}

int lomuto(vector<int>&arr, int ini, int fim, int& num_trocas_chamadas) {
    int P_index = randomPivotIndex(arr, ini, fim);
    swap(arr[P_index], arr[fim]);
    num_trocas_chamadas++;
    
    int P = arr[fim]; // def nosso pivot
    int i = ini - 1, j = ini;

    for(j = ini; j < fim; j++)
        if(arr[j] <= P) {
            swap(arr[++i], arr[j]);  
            num_trocas_chamadas++;
        }

    // The pivot is in its position now
    swap(arr[++i], arr[j]);
    num_trocas_chamadas++;

    // Return the pivot index
    return i;
}

void quicksort(vector<int>&arr, int ini, int fim, int& num_trocas_chamadas) {
    num_trocas_chamadas++;
    if (ini < fim) {
        int pivo = lomuto(arr, ini, fim, num_trocas_chamadas);

        quicksort(arr, ini, pivo - 1, num_trocas_chamadas);
        quicksort(arr, pivo + 1, fim, num_trocas_chamadas);
    }
}

int main() {
    vector<int> arr = {-23, 10, 7, -34, 432, 3};

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