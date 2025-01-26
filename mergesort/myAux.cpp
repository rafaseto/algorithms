#include <iostream>
#include <vector>
#include <string>
#include <cmath>

struct Container {
    std::string codigo;
    std::string cnpj;
    int peso;
};

struct Triagem {
    std::string codigo;
    std::string causa;
    int prioridade; // 1 para CNPJ, 2 para peso
    int indice;     // Índice para manter a ordem dos cadastrados
};

double calculaDiferencaPercentual(int peso1, int peso2) {
    return std::round(std::abs(peso1 - peso2) * 100.0 / peso1);
}

void merge(std::vector<Triagem>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    std::vector<Triagem> leftArr(n1), rightArr(n2);

    for (int i = 0; i < n1; ++i)
        leftArr[i] = arr[left + i];
    for (int i = 0; i < n2; ++i)
        rightArr[i] = arr[mid + 1 + i];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        // Comparação por prioridade, índice dos cadastrados para desempate
        if (leftArr[i].prioridade < rightArr[j].prioridade || 
            (leftArr[i].prioridade == rightArr[j].prioridade && leftArr[i].indice < rightArr[j].indice)) {
            arr[k] = leftArr[i];
            i++;
        } else {
            arr[k] = rightArr[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = leftArr[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = rightArr[j];
        j++;
        k++;
    }
}

void mergeSort(std::vector<Triagem>& arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);

        merge(arr, left, mid, right);
    }
}

int main() {
    int nCadastrados;
    std::cin >> nCadastrados;

    std::vector<Container> cadastrados(nCadastrados);
    for (int i = 0; i < nCadastrados; ++i) {
        std::cin >> cadastrados[i].codigo >> cadastrados[i].cnpj >> cadastrados[i].peso;
    }

    int nInspecionados;
    std::cin >> nInspecionados;

    std::vector<Container> inspecionados(nInspecionados);
    for (int i = 0; i < nInspecionados; ++i) {
        std::cin >> inspecionados[i].codigo >> inspecionados[i].cnpj >> inspecionados[i].peso;
    }

    std::vector<Triagem> resultados;

    for (size_t i = 0; i < cadastrados.size(); ++i) {
        const auto& cad = cadastrados[i];
        for (const auto& insp : inspecionados) {
            if (insp.codigo == cad.codigo) {
                if (insp.cnpj != cad.cnpj) {
                    resultados.push_back({insp.codigo, cad.cnpj + "<->" + insp.cnpj, 1, (int)i});
                } else {
                    double diferenca = calculaDiferencaPercentual(cad.peso, insp.peso);
                    if (diferenca > 10) {
                        resultados.push_back({insp.codigo, std::to_string(std::abs(cad.peso - insp.peso)) + "kg(" + std::to_string((int)diferenca) + "%)", 2, (int)i});
                    }
                }
                break;
            }
        }
    }

    // Ordenar resultados usando MergeSort
    mergeSort(resultados, 0, resultados.size() - 1);

    // Imprimir resultados
    for (const auto& res : resultados) {
        std::cout << res.codigo << ":" << res.causa << "\n";
    }

    return 0;
}
