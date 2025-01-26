#include <iostream>
#include <string>
#include <fstream>
using namespace std;

struct Container {
    string codigo;
    string cnpj;
    int32_t peso;
    int32_t ordemCadastro;
};

struct Fiscalizacao {
    Container containerCadastrado;
    Container container;
    int32_t prioridade;
    int32_t diferencaPeso;
};

int32_t arredondarSimples(double numero) {
    if (numero >= 0) {
        return static_cast<int>(numero + 0.5); // Arredonda para cima
    } else {
        return static_cast<int>(numero - 0.5); // Arredonda para baixo
    }
}

// func para checar se o container ta ok
bool containerOk(const Container& cadastrado, const Container& selecionado) {
    if (cadastrado.cnpj != selecionado.cnpj) 
        return false;
    else if (selecionado.peso > cadastrado.peso * 1.1)
        return false;
    else 
        return true;
}

Container separaContaineresNaoOk(Container* cadastrados, int32_t qtdeCadastrados, Container* selecionados, int32_t qtdeSelecionados, Fiscalizacao* containeresNaoOk, int32_t& qtdeNaoOk) {
    int32_t k = 0;

    for (int32_t i =  0; i < qtdeSelecionados; i++) {
        for (int32_t j = 0; j < qtdeCadastrados; j++) {
            if (selecionados[i].codigo == cadastrados[j].codigo) {
                if (!containerOk(cadastrados[j], selecionados[i])) {
                    int32_t prioridade = (cadastrados[j].cnpj != selecionados[i].cnpj) ? 1 : 2;
                    int32_t diferencaPeso = arredondarSimples(abs(selecionados[i].peso - cadastrados[j].peso) * 100 / (double) cadastrados[j].peso);
                    containeresNaoOk[k++] = {
                        cadastrados[j],
                        selecionados[i], 
                        prioridade,
                        diferencaPeso
                    };
                    qtdeNaoOk++;
                }
                break;
            }
        }
    }
}

// Função para copiar elementos de um vetor para outro
void copiar(Fiscalizacao* destino, Fiscalizacao* origem, int32_t tamanho) {
    std::copy(origem, origem + tamanho, destino);
}

// Função para intercalar dois subarrays ordenados
void intercalar(Fiscalizacao* vetorAux, Fiscalizacao* vetorOriginal, int32_t iniSubArr01, int32_t fimSubArr01, int32_t fimSubArr02) {
    int32_t i1 = iniSubArr01;     // Índice do primeiro subarray
    int32_t i2 = fimSubArr01 + 1; // Índice do segundo subarray
    int32_t k = iniSubArr01;      // Índice do vetor auxiliar








    // Intercala os elementos dos subarrays ordenados
    while (i1 <= fimSubArr01 && i2 <= fimSubArr02) {
        // CNPJ DIVERGENTE TEM PRIORIDADE
        if (vetorOriginal[i1].prioridade < vetorOriginal[i2].prioridade)
            vetorAux[k++] = vetorOriginal[i1++];

        // MESMO GRAU DE PRIORIDADE
        else if (vetorOriginal[i1].prioridade == vetorOriginal[i2].prioridade)

            // AMBOS COM CNPJ DIVERGENTE --> VER ORDEM DO CADASTRO
            if (vetorOriginal[i1].prioridade == 1) {

                // ORDEM DE CADASTRO MAIS PRIORITARIA 
                if (vetorOriginal[i1].containerCadastrado.ordemCadastro < vetorOriginal[i2].containerCadastrado.ordemCadastro){
                    cout << vetorOriginal[i1].container.ordemCadastro;
                    vetorAux[k++] = vetorOriginal[i1++];}
                else 
                    vetorAux[k++] = vetorOriginal[i2++];

            }
    
            // AMBOS COM DIFF GRANDE DE PESO
            else {
                
                // DIFERENCA DE PESO MAIOR, LOGO MAIS PRIORITARIA
                if (vetorOriginal[i1].diferencaPeso > vetorOriginal[i2].diferencaPeso) {
                    vetorAux[k++] = vetorOriginal[i1++];
                }

                // MESMA DIFF DE PESO --> VER ORDEM DO CADASTRO
                else if (vetorOriginal[i1].diferencaPeso == vetorOriginal[i2].diferencaPeso) {

                    // ORDEM DE CADASTRO MAIS PRIORITARIA 
                    if (vetorOriginal[i1].containerCadastrado.ordemCadastro < vetorOriginal[i2].containerCadastrado.ordemCadastro)
                        vetorAux[k++] = vetorOriginal[i1++];
                    else 
                        vetorAux[k++] = vetorOriginal[i2++];

                }

                // DIFERENCA DE PESO MENOR
                else 
                    vetorAux[k++] = vetorOriginal[i2++];

            }

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
void mergesort(Fiscalizacao* vetorAux, Fiscalizacao* vetorOriginal, int32_t iniSubArr01, int32_t fimSubArr02) {
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
int main(int argc, char* argv[]) {

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

    int32_t numCadastrados;
    input >> numCadastrados;

    Container cadastrados[numCadastrados];
    for (int i = 0; i < numCadastrados; i++) {
        input >> cadastrados[i].codigo; 
        input >> cadastrados[i].cnpj; 
        input >> cadastrados[i].peso; 
        cadastrados[i].ordemCadastro = i;
    }

    int32_t numSelecionados;
    input >> numSelecionados;

    Container selecionados[numSelecionados];
    for (int i = 0; i < numSelecionados; i++) {
        input >> selecionados[i].codigo;
        input >> selecionados[i].cnpj;
        input >> selecionados[i].peso;
    }

    Fiscalizacao containeresNaoOk[numSelecionados];
    Fiscalizacao aux[numSelecionados];
    int32_t numNaoOk = 0;

    separaContaineresNaoOk(cadastrados, numCadastrados, selecionados, numSelecionados, containeresNaoOk, numNaoOk);

    mergesort(aux, containeresNaoOk, 0, numNaoOk-1);
    
    for (int i = 0; i < numNaoOk; i++) {
        if (containeresNaoOk[i].prioridade == 1) {
            output 
            << containeresNaoOk[i].container.codigo 
            << ":" 
            << containeresNaoOk[i].containerCadastrado.cnpj 
            << "<->" 
            << containeresNaoOk[i].container.cnpj 
            << "\n";
        } else {
            output 
            << containeresNaoOk[i].container.codigo 
            << ":" 
            << abs(containeresNaoOk[i].container.peso - containeresNaoOk[i].containerCadastrado.peso) 
            << "kg(" 
            << containeresNaoOk[i].diferencaPeso 
            << "%)"
            << "\n";  
        }
    }

    return 0;
}