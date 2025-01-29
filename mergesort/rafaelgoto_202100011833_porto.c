#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct {
    char codigo[50];
    char cnpj[50];
    int peso;
    int ordemCadastro;
} Container;

typedef struct {
    Container containerCadastrado;
    Container container;
    int prioridade;
    int diferencaPeso;
} Fiscalizacao;

int arredondarSimples(double numero) {
    if (numero >= 0) {
        return (int)(numero + 0.5);
    } else {
        return (int)(numero - 0.5);
    }
}

int containerOk(const Container* cadastrado, const Container* selecionado) {
    double limiteSuperior = cadastrado->peso * 1.105;
    double limiteInferior = cadastrado->peso * 0.895;
    return (strcmp(cadastrado->cnpj, selecionado->cnpj) == 0) &&
           (selecionado->peso <= limiteSuperior && selecionado->peso >= limiteInferior);
}

void separaContaineresNaoOk(Container* cadastrados, int qtdeCadastrados, Container* selecionados, int qtdeSelecionados, Fiscalizacao* containeresNaoOk, int* qtdeNaoOk) {
    int k = 0;
    for (int i = 0; i < qtdeSelecionados; i++) {
        for (int j = 0; j < qtdeCadastrados; j++) {
            if (strcmp(selecionados[i].codigo, cadastrados[j].codigo) == 0) {
                if (!containerOk(&cadastrados[j], &selecionados[i])) {
                    int prioridade = (strcmp(cadastrados[j].cnpj, selecionados[i].cnpj) != 0) ? 1 : 2;
                    int diferencaPeso = arredondarSimples(fabs(selecionados[i].peso - cadastrados[j].peso) * 100.0 / cadastrados[j].peso);
                    containeresNaoOk[k].containerCadastrado = cadastrados[j];
                    containeresNaoOk[k].container = selecionados[i];
                    containeresNaoOk[k].prioridade = prioridade;
                    containeresNaoOk[k].diferencaPeso = diferencaPeso;
                    k++;
                    (*qtdeNaoOk)++;
                }
                break;
            }
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
    Container* cadastrados = malloc(numCadastrados * sizeof(Container));
    for (int i = 0; i < numCadastrados; i++) {
        fscanf(input, "%s %s %d", cadastrados[i].codigo, cadastrados[i].cnpj, &cadastrados[i].peso);
        cadastrados[i].ordemCadastro = i;
    }

    fscanf(input, "%d", &numSelecionados);
    Container* selecionados = malloc(numSelecionados * sizeof(Container));
    for (int i = 0; i < numSelecionados; i++) {
        fscanf(input, "%s %s %d", selecionados[i].codigo, selecionados[i].cnpj, &selecionados[i].peso);
    }

    Fiscalizacao* containeresNaoOk = malloc(numSelecionados * sizeof(Fiscalizacao));
    Fiscalizacao* aux = malloc(numSelecionados * sizeof(Fiscalizacao));
    
    separaContaineresNaoOk(cadastrados, numCadastrados, selecionados, numSelecionados, containeresNaoOk, &numNaoOk);
    mergesort(aux, containeresNaoOk, 0, numNaoOk - 1);
    for (int i = 0; i < numNaoOk; i++) {
        if (containeresNaoOk[i].prioridade == 1) {
            fprintf(output, "%s:%s<->%s\n", containeresNaoOk[i].container.codigo, containeresNaoOk[i].containerCadastrado.cnpj, containeresNaoOk[i].container.cnpj);
        } else {
            fprintf(output, "%s:%dkg(%d%%)\n", containeresNaoOk[i].container.codigo, abs(containeresNaoOk[i].container.peso - containeresNaoOk[i].containerCadastrado.peso), containeresNaoOk[i].diferencaPeso);
        }
    }

    free(cadastrados);
    free(selecionados);
    free(containeresNaoOk);
    free(aux);
    
    return 0;
}