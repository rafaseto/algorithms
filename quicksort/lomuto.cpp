#include <iostream>
#include <vector>
#include <algorithm> 
using namespace std;

int lomuto(vector<int>&arr, int ini, int fim) {
    int P = arr[fim], i = ini - 1, j = ini;

    for(j = ini; j < fim; j++)
        if(arr[j] <= P) swap(arr[++i], arr[j]);

    // The pivot is in its position now
    swap(arr[++i], arr[j]);

    // Return the pivot index
    return i;
}

void quicksort(vector<int>&arr, int ini, int fim) {
    if (ini < fim) {
        int pivo = lomuto(arr, ini, fim);

        quicksort(arr, ini, pivo - 1);
        quicksort(arr, pivo + 1, fim);
    }
}

int main() {
    vector<int> arr = {8, 2, 4, 7, 1, 3, 9, 6, 5};

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

    return 0;
}