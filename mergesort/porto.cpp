#include <iostream>
#include <algorithm>
#include <string>
using namespace std;

struct Container {
    string codigo;
    string cnpj;
    int32_t peso;
    int32_t ordemCadastro;
};

// func para checar se o container ta ok
bool containerOk(const Container& cadastrado, const Container& selecionado) {
    if (cadastrado.cnpj != selecionado.cnpj) 
        return false;
    else if (selecionado.peso > cadastrado.peso * 1.1)
        return false;
    else 
        return true;
}

Container separaContaineresNaoOk(Container* cadastrados, int32_t qtdeCadastrados, Container* selecionados, int32_t qtdeSelecionados, Container* containeresNaoOk) {
    int32_t k = 0;

    for (int32_t i =  0; i < qtdeSelecionados; i++) {
        for (int32_t j = 0; j < qtdeCadastrados; j++) {
            if (selecionados[i].codigo == cadastrados[j].codigo) {
                if (!containerOk(cadastrados[j], selecionados[i])) {
                    int32_t prioridade = (cadastrados[j].cnpj != selecionados[i].cnpj) ? 1 : 2;
                    int32_t diferencaPeso = abs(selecionados[i].peso - cadastrados[j].peso) / cadastrados[j].peso;
                    containeresNaoOk[k++] = {
                        selecionados[i].codigo,
                        selecionados[i].cnpj, 
                        selecionados[i].peso, 
                        selecionados[i].ordemCadastro
                    };
                }
                break;
            }
        }
    }
}

// Função para copiar elementos de um vetor para outro
void copiar(int32_t* destino, int32_t* origem, int32_t tamanho) {
    std::copy(origem, origem + tamanho, destino);
}

// Função para intercalar dois subarrays ordenados
void intercalar(int32_t* vetorAux, int32_t* vetorOriginal, int32_t iniSubArr01, int32_t fimSubArr01, int32_t fimSubArr02) {
    int32_t i1 = iniSubArr01;     // Índice do primeiro subarray
    int32_t i2 = fimSubArr01 + 1; // Índice do segundo subarray
    int32_t k = iniSubArr01;      // Índice do vetor auxiliar

    // Intercala os elementos dos subarrays ordenados
    while (i1 <= fimSubArr01 && i2 <= fimSubArr02) {
        if (vetorOriginal[i1] <= vetorOriginal[i2])
            vetorAux[k++] = vetorOriginal[i1++];
        else
            vetorAux[k++] = vetorOriginal[i2++];
    }

    // Copia os elementos restantes do primeiro subarray, se houver
    if (i1 <= fimSubArr01) {
        copiar(&vetorAux[k], &vetorOriginal[i1], fimSubArr01 - i1 + 1);
    } 
    // Copia os elementos restantes do segundo subarray, se houver
    else {
        copiar(&vetorAux[k], &vetorOriginal[i2], fimSubArr02 - i2 + 1);
    }

    // Copia os elementos intercalados de volta para o vetor original
    copiar(&vetorOriginal[iniSubArr01], &vetorAux[iniSubArr01], fimSubArr02 - iniSubArr01 + 1);
}

// Função Merge Sort recursiva
void mergesort(int32_t* vetorAux, int32_t* vetorOriginal, int32_t iniSubArr01, int32_t fimSubArr02) {
    if (iniSubArr01 < fimSubArr02) {
        int32_t fimSubArr01 = iniSubArr01 + (fimSubArr02 - iniSubArr01) / 2;

        // Divide o vetor em duas partes e ordena cada parte
        mergesort(vetorAux, vetorOriginal, iniSubArr01, fimSubArr01);
        mergesort(vetorAux, vetorOriginal, fimSubArr01 + 1, fimSubArr02);

        // Intercala as duas partes ordenadas
        intercalar(vetorAux, vetorOriginal, iniSubArr01, fimSubArr01, fimSubArr02);
    }
}

// Função principal para testar o Merge Sort
int main() {
    Container cadastrados[6];
    Container selecionados[5];
    Container containeresNaoOK[6];

    cadastrados[0] = {"QOZJ7913219", "34.699.211/9365-11", 13822, 0};
    cadastrados[1] = {"FCCU4584578", "50.503.434/5731-28", 16022, 1};
    cadastrados[2] = {"KTAJ0603546", "20.500.522/6013-58", 25279, 2};
    cadastrados[3] = {"ZYHU3978783", "43.172.263/4442-14", 24543, 3};
    cadastrados[4] = {"IKQZ7582839", "51.743.446/1183-18", 12160, 4};
    cadastrados[5] = {"HAAZ0273059", "25.699.428/4746-79", 16644, 5};

    selecionados[0] = {"ZYHU3978783", "43.172.263/4442-14", 29765, 3};
    selecionados[1] = {"IKQZ7582839", "51.743.446/1113-18", 18501, 4};
    selecionados[2] = {"KTAJ0603546", "20.500.522/6113-58", 17842, 2};
    selecionados[3] = {"QOZJ7913219", "34.699.211/9365-11", 16722, 0};
    selecionados[4] = {"FCCU4584578", "50.503.434/5731-28", 16398, 1};

    separaContaineresNaoOk(cadastrados, 6, selecionados, 5, containeresNaoOK);
    
    for (Container container : containeresNaoOK) {
        cout << container.codigo << " - " << container.ordemCadastro << "\n";
    }


    return 0;
}
