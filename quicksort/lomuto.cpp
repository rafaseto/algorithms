#include <iostream>
#include <vector>
#include <algorithm> 
using namespace std;

int lomuto(vector<int>&arr, int ini, int fim, int& num_trocas_chamadas) {
    int P = arr[fim], i = ini - 1, j = ini;

    for(j = ini; j < fim; j++)
        if(arr[j] <= P) {
            cout << endl;
            cout << arr[j] << " is smaller than the pivot " << P << " (swapping)";
            swap(arr[++i], arr[j]);  
            cout << endl;

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
    vector<int> arr = {847, 38, -183, -13, 94, -2, -42, 54, 28, 100};

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