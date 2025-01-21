#include <iostream>
#include <vector>
#include <algorithm> 
using namespace std;

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

int hoare(vector<int>&arr, int ini, int fim, int& num_trocas_chamadas) {
    // Determina o índice do pivô com a técnica de mediana de três
    int P_index = medianOfThree(arr, ini, fim);
    swap(arr[P_index], arr[ini]); // Move o pivô para o início
    num_trocas_chamadas++;

    int P = arr[ini]; // definindo o pivo
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