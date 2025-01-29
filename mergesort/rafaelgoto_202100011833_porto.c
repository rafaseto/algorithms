#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define TABLE_SIZE 1009 

typedef struct Container {
    char codigo[50];
    char cnpj[50];
    int peso;
    int ordemCadastro;
    struct Container* next; 
} Container;

typedef struct {
    Container containerCadastrado;
    Container container;
    int prioridade;
    int diferencaPeso;
} Fiscalizacao;


Container* hashTable[TABLE_SIZE] = { NULL };


unsigned int hash(const char* str) {
    unsigned int hashValue = 0;
    while (*str) {
        hashValue = (hashValue * 31) + (*str++); 
    }
    return hashValue % TABLE_SIZE;
}

// Insere container na tabela 
void inserirHash(Container* container) {
    unsigned int index = hash(container->codigo);
    container->next = hashTable[index];
    hashTable[index] = container;
}

// Busca container pelo código na tabela hash
Container* buscarHash(const char* codigo) {
    unsigned int index = hash(codigo);
    Container* atual = hashTable[index];
    while (atual) {
        if (strcmp(atual->codigo, codigo) == 0) {
            return atual; 
        }
        atual = atual->next;
    }
    return NULL; 
}

int arredondarSimples(double numero) {
    return (numero >= 0) ? (int)(numero + 0.5) : (int)(numero - 0.5);
}

int containerOk(const Container* cadastrado, const Container* selecionado) {
    double limiteSuperior = cadastrado->peso * 1.105;
    double limiteInferior = cadastrado->peso * 0.895;
    return (strcmp(cadastrado->cnpj, selecionado->cnpj) == 0) &&
           (selecionado->peso <= limiteSuperior && selecionado->peso >= limiteInferior);
}

void separaContaineresNaoOk(Container* selecionados, int qtdeSelecionados, Fiscalizacao* containeresNaoOk, int* qtdeNaoOk) {
    int k = 0;
    for (int i = 0; i < qtdeSelecionados; i++) {
        Container* encontrado = buscarHash(selecionados[i].codigo); // Busca O(1)
        if (encontrado && !containerOk(encontrado, &selecionados[i])) {
            int prioridade = (strcmp(encontrado->cnpj, selecionados[i].cnpj) != 0) ? 1 : 2;
            int diferencaPeso = arredondarSimples(fabs(selecionados[i].peso - encontrado->peso) * 100.0 / encontrado->peso);
            containeresNaoOk[k].containerCadastrado = *encontrado;
            containeresNaoOk[k].container = selecionados[i];
            containeresNaoOk[k].prioridade = prioridade;
            containeresNaoOk[k].diferencaPeso = diferencaPeso;
            k++;
            (*qtdeNaoOk)++;
        }
    }
}

void copiar(Fiscalizacao* destino, Fiscalizacao* origem, int tamanho) {
    memcpy(destino, origem, tamanho * sizeof(Fiscalizacao));
}

void intercalar(Fiscalizacao* vetorAux, Fiscalizacao* vetorOriginal, int iniSubArr01, int fimSubArr01, int fimSubArr02) {
    int i1 = iniSubArr01, i2 = fimSubArr01 + 1, k = iniSubArr01;
    while (i1 <= fimSubArr01 && i2 <= fimSubArr02) {
        if (vetorOriginal[i1].prioridade < vetorOriginal[i2].prioridade) {
            vetorAux[k++] = vetorOriginal[i1++];
        } else if (vetorOriginal[i1].prioridade > vetorOriginal[i2].prioridade) {
            vetorAux[k++] = vetorOriginal[i2++];
        } else {
            if (vetorOriginal[i1].prioridade == 1) {
                vetorAux[k++] = (vetorOriginal[i1].containerCadastrado.ordemCadastro < vetorOriginal[i2].containerCadastrado.ordemCadastro)
                                 ? vetorOriginal[i1++] : vetorOriginal[i2++];
            } else {
                if (vetorOriginal[i1].diferencaPeso > vetorOriginal[i2].diferencaPeso) {
                    vetorAux[k++] = vetorOriginal[i1++];
                } else if (vetorOriginal[i1].diferencaPeso < vetorOriginal[i2].diferencaPeso) {
                    vetorAux[k++] = vetorOriginal[i2++];
                } else {
                    vetorAux[k++] = (vetorOriginal[i1].containerCadastrado.ordemCadastro < vetorOriginal[i2].containerCadastrado.ordemCadastro)
                                     ? vetorOriginal[i1++] : vetorOriginal[i2++];
                }
            }
        }
    }
    if (i1 <= fimSubArr01) copiar(&vetorAux[k], &vetorOriginal[i1], fimSubArr01 - i1 + 1);
    else copiar(&vetorAux[k], &vetorOriginal[i2], fimSubArr02 - i2 + 1);
    copiar(&vetorOriginal[iniSubArr01], &vetorAux[iniSubArr01], fimSubArr02 - iniSubArr01 + 1);
}

void mergesort(Fiscalizacao* vetorAux, Fiscalizacao* vetorOriginal, int iniSubArr01, int fimSubArr02) {
    if (iniSubArr01 < fimSubArr02) {
        int fimSubArr01 = iniSubArr01 + (fimSubArr02 - iniSubArr01) / 2;
        mergesort(vetorAux, vetorOriginal, iniSubArr01, fimSubArr01);
        mergesort(vetorAux, vetorOriginal, fimSubArr01 + 1, fimSubArr02);
        intercalar(vetorAux, vetorOriginal, iniSubArr01, fimSubArr01, fimSubArr02);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <arquivo_entrada> <arquivo_saida>\n", argv[0]);
        return 1;
    }

    FILE* input = fopen(argv[1], "r");
    FILE* output = fopen(argv[2], "w");
    if (!input || !output) {
        fprintf(stderr, "Erro ao abrir os arquivos.\n");
        return 1;
    }

    int numCadastrados, numSelecionados, numNaoOk = 0;

    fscanf(input, "%d", &numCadastrados);
    for (int i = 0; i < numCadastrados; i++) {
        Container* novo = (Container*)malloc(sizeof(Container));
        fscanf(input, "%s %s %d", novo->codigo, novo->cnpj, &novo->peso);
        novo->ordemCadastro = i;
        novo->next = NULL;
        inserirHash(novo);
    }

    fscanf(input, "%d", &numSelecionados);
    Container* selecionados = (Container*)malloc(numSelecionados * sizeof(Container));
    for (int i = 0; i < numSelecionados; i++) {
        fscanf(input, "%s %s %d", selecionados[i].codigo, selecionados[i].cnpj, &selecionados[i].peso);
    }

    Fiscalizacao* containeresNaoOk = (Fiscalizacao*)malloc(numSelecionados * sizeof(Fiscalizacao));
    Fiscalizacao* aux = (Fiscalizacao*)malloc(numSelecionados * sizeof(Fiscalizacao));
    
    separaContaineresNaoOk(selecionados, numSelecionados, containeresNaoOk, &numNaoOk);
    mergesort(aux, containeresNaoOk, 0, numNaoOk - 1);

    for (int i = 0; i < numNaoOk; i++) {
        if (containeresNaoOk[i].prioridade == 1) {
            fprintf(output, "%s:%s<->%s\n", containeresNaoOk[i].container.codigo, containeresNaoOk[i].containerCadastrado.cnpj, containeresNaoOk[i].container.cnpj);
        } else {
            fprintf(output, "%s:%dkg(%d%%)\n", containeresNaoOk[i].container.codigo, abs(containeresNaoOk[i].container.peso - containeresNaoOk[i].containerCadastrado.peso), containeresNaoOk[i].diferencaPeso);
        }
    }

    free(selecionados);
    free(containeresNaoOk);
    free(aux);
    fclose(input);
    fclose(output);
    
    return 0;
}
