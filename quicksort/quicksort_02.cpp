#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>

using namespace std;

int randomPivotIndex(int* arr, int ini, int fim) {
    int size = fim - ini + 1;
    return ini + abs(arr[ini]) % size;
}

int medianOfThree(int* arr, int ini, int fim) {
    int size = fim - ini + 1;
    int i = ini + size / 4;
    int j = ini + size / 2;
    int k = ini + 3 * size / 4;

    int a = arr[i], b = arr[j], c = arr[k];

    if ((a < b && a > c) || (a > b && a < c)) return i;
    else if ((b < a && b > c) || (b > a && b < c)) return j;
    else return k;
}

void swap(int& a, int& b) {
    int temp = a;
    a = b;
    b = temp;
}

int lomuto(int* arr, int ini, int fim, int& num_trocas_chamadas, string tipo) {
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

    swap(arr[++i], arr[fim]);
    num_trocas_chamadas++;

    return i;
}

int hoare(int* arr, int ini, int fim, int& num_trocas_chamadas, string tipo) {
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

    while (true) {
        while (arr[--j] > P);
        while (arr[++i] < P);
        if (i < j) {
            swap(arr[i], arr[j]);
            num_trocas_chamadas++;
        } else return j;
    }
}

void quicksort(int* arr, int ini, int fim, int& num_trocas_chamadas, string tipo, string metodo) {
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

int main(int argc, char* argv[]) {
    // Verifica se os arquivos de entrada e saída foram fornecidos
    if (argc < 3) {
        cerr << "Uso: " << argv[0] << " <arquivo_entrada> <arquivo_saida>" << endl;
        return 1;
    }

    // Abre os arquivos de entrada e saída
    ifstream input(argv[1]);
    ofstream output(argv[2]);

    if (!input.is_open() || !output.is_open()) {
        cerr << "Erro ao abrir os arquivos de entrada ou saída." << endl;
        return 1;
    }

    int num_vetores;
    input >> num_vetores;

    // Aloca os vetores dinamicamente
    int** vetores = new int*[num_vetores];
    int* tamanhos = new int[num_vetores];

    for (int i = 0; i < num_vetores; i++) {
        input >> tamanhos[i];
        vetores[i] = new int[tamanhos[i]];
        for (int j = 0; j < tamanhos[i]; j++) {
            input >> vetores[i][j];
        }
    }

    string metodos[2] = {"lomuto", "hoare"};
    string tipos[3] = {"standard", "median", "random"};
    string abreviacoes[6] = {"LP", "LM", "LA", "HP", "HM", "HA"};
    int ordem_abreviacoes[6] = {0, 1, 2, 3, 4, 5}; // Ordem: LP, LM, LA, HP, HM, HA

    for (int idx = 0; idx < num_vetores; idx++) {
        output << idx << ":N(" << tamanhos[idx] << ")";

        int resultados[6];
        string resultados_abreviacoes[6];

        for (int m = 0; m < 2; m++) {
            for (int t = 0; t < 3; t++) {
                int* arr_copy = new int[tamanhos[idx]];
                for (int i = 0; i < tamanhos[idx]; i++) {
                    arr_copy[i] = vetores[idx][i];
                }

                int num_trocas = 0;
                quicksort(arr_copy, 0, tamanhos[idx] - 1, num_trocas, tipos[t], metodos[m]);

                int index = m * 3 + t;
                resultados[index] = num_trocas;
                resultados_abreviacoes[index] = abreviacoes[index];

                delete[] arr_copy;
            }
        }

        // Ordena os resultados
        for (int i = 0; i < 6; i++) {
            for (int j = i + 1; j < 6; j++) {
                if (resultados[i] > resultados[j] || 
                    (resultados[i] == resultados[j] && ordem_abreviacoes[i] > ordem_abreviacoes[j])) {
                    swap(resultados[i], resultados[j]);
                    swap(resultados_abreviacoes[i], resultados_abreviacoes[j]);
                }
            }
        }

        for (int i = 0; i < 6; i++) {
            output << "," << resultados_abreviacoes[i] << "(" << resultados[i] << ")";
        }
        output << endl;
    }

    // Libera a memória alocada
    for (int i = 0; i < num_vetores; i++) {
        delete[] vetores[i];
    }
    delete[] vetores;
    delete[] tamanhos;

    return 0;
}