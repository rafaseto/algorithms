#include <iostream>
#include <vector>
#include <algorithm> 
using namespace std;


int hoare(vector<int>&arr, int ini, int fim) {
    int P = arr[ini], i = ini - 1, j = fim + 1;

    while (1) {
        while (arr[--j] > P);
        while (arr[++i] < P);
        if (i < j) swap(arr[i], arr[j]);
        else return j;
    }
}

void quicksort(vector<int>&arr, int ini, int fim) {
    if (ini < fim) {
        int pivo = hoare(arr, ini, fim);

        quicksort(arr, ini, pivo);
        quicksort(arr, pivo + 1, fim);
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

    quicksort(arr, 0 , arr.size() - 1);

    cout << "Ordered vector: ";
    for (int num : arr) {
        cout << num << " ";
    }
    cout << endl;

    // Exibir as estatísticas
    // cout << "Swaps and Calls: " << num_trocas_chamadas << endl;

    return 0;
}