#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <sstream>
#include <map>
#include <tuple>
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
    int num_vetores;
    cin >> num_vetores;
    vector<vector<int>> vetores(num_vetores);

    for (int i = 0; i < num_vetores; i++) {
        int tamanho;
        cin >> tamanho;
        vetores[i].resize(tamanho);
        for (int j = 0; j < tamanho; j++) {
            cin >> vetores[i][j];
        }
    }

    vector<string> metodos = {"lomuto", "hoare"};
    vector<string> tipos = {"standard", "median", "random"};
    map<string, string> abreviacoes = {
        {"lomuto-padrão", "LP"},
        {"lomuto-mediana", "LM"},
        {"lomuto-aleatorio", "LA"},
        {"hoare-padrão", "HP"},
        {"hoare-mediana", "HM"},
        {"hoare-aleatorio", "HA"}
    };

    for (int idx = 0; idx < num_vetores; idx++) {
        vector<int> arr = vetores[idx];
        cout << idx << ":N(" << arr.size() << ")";
        
        vector<pair<int, string>> resultados;
        for (const auto& metodo : metodos) {
            for (const auto& tipo : tipos) {
                vector<int> arr_copy = arr;
                int num_trocas = 0;
                quicksort(arr_copy, 0, arr.size() - 1, num_trocas, tipo, metodo);
                string chave = metodo + "-" + tipo;
                resultados.push_back({num_trocas, abreviacoes[chave]});
            }
        }

        // Ordena resultados com base no número de trocas e critério de desempate
        sort(resultados.begin(), resultados.end(), [](const pair<int, string>& a, const pair<int, string>& b) {
            if (a.first != b.first) return a.first < b.first;
            string ordem = "LP,LM,LA,HP,HM,HA";
            return ordem.find(a.second) < ordem.find(b.second);
        });

        for (const auto& res : resultados) {
            cout << "," << res.second << "(" << res.first << ")";
        }
        cout << endl;
    }

    return 0;
}