#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_PLATE 32
#define MAX_CODE 64

typedef struct {
    char plate[MAX_PLATE];
    int max_weight;
    int max_volume;
} Truck;

typedef struct {
    char code[MAX_CODE];
    double value;
    int weight;
    int volume;
    int used;    // 0: disponível; 1: já alocado
    int index;   // ordem de entrada
} Product;

// Função para comparar índices (ordem de entrada)
int cmp_int(const void *a, const void *b) {
    int ia = *(const int *)a;
    int ib = *(const int *)b;
    return ia - ib;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s arquivo_entrada arquivo_saida\n", argv[0]);
        return 1;
    }
    
    FILE *fin = fopen(argv[1], "r");
    if (!fin) {
        fprintf(stderr, "Erro ao abrir arquivo de entrada.\n");
        return 1;
    }
    
    FILE *fout = fopen(argv[2], "w");
    if (!fout) {
        fprintf(stderr, "Erro ao abrir arquivo de saída.\n");
        fclose(fin);
        return 1;
    }
    
    int numTrucks;
    fscanf(fin, "%d", &numTrucks);
    Truck *trucks = malloc(numTrucks * sizeof(Truck));
    for (int i = 0; i < numTrucks; i++) {
        fscanf(fin, "%s %d %d", trucks[i].plate, &trucks[i].max_weight, &trucks[i].max_volume);
    }
    
    int numProducts;
    fscanf(fin, "%d", &numProducts);
    Product *products = malloc(numProducts * sizeof(Product));
    for (int i = 0; i < numProducts; i++) {
        fscanf(fin, "%s %lf %d %d", products[i].code, &products[i].value, &products[i].weight, &products[i].volume);
        products[i].used = 0;
        products[i].index = i;
    }
    
    // Processa cada caminhão separadamente
    for (int t = 0; t < numTrucks; t++) {
        int W = trucks[t].max_weight;
        int V = trucks[t].max_volume;
        
        // Reúne os índices dos produtos disponíveis para este caminhão
        int m = 0;
        for (int i = 0; i < numProducts; i++) {
            if (!products[i].used) {
                m++;
            }
        }
        int *avail = malloc(m * sizeof(int));
        int pos = 0;
        for (int i = 0; i < numProducts; i++) {
            if (!products[i].used) {
                avail[pos++] = i;
            }
        }
        
        // Dimensões para a DP: (m+1) x (W+1) x (V+1)
        int dim1 = m + 1;
        int dim2 = W + 1;
        int dim3 = V + 1;
        size_t totalSize = (size_t) dim1 * dim2 * dim3;
        
        // Aloca blocos contíguos para dp (double) e dec (int)
        double *dp = malloc(totalSize * sizeof(double));
        int *dec = malloc(totalSize * sizeof(int));
        // Inicializa as tabelas com zero
        memset(dp, 0, totalSize * sizeof(double));
        memset(dec, 0, totalSize * sizeof(int));
        
        // Macro para acessar o índice: dp[i][w][v]
        #define IDX(i, w, v) ((i) * dim2 * dim3 + (w) * dim3 + (v))
        
        // DP: itera sobre os itens disponíveis (1..m)
        for (int i = 1; i <= m; i++) {
            int idxProd = avail[i - 1]; // índice global do produto
            int pweight = products[idxProd].weight;
            int pvolume = products[idxProd].volume;
            double pvalue = products[idxProd].value;
            for (int w = 0; w < dim2; w++) {
                for (int v = 0; v < dim3; v++) {
                    // Copia o valor sem usar o produto i
                    dp[IDX(i, w, v)] = dp[IDX(i - 1, w, v)];
                    dec[IDX(i, w, v)] = 0;
                    // Se couber o produto, tenta incluí-lo
                    if (w >= pweight && v >= pvolume) {
                        double candidate = dp[IDX(i - 1, w - pweight, v - pvolume)] + pvalue;
                        if (candidate > dp[IDX(i, w, v)]) {
                            dp[IDX(i, w, v)] = candidate;
                            dec[IDX(i, w, v)] = 1;
                        }
                    }
                }
            }
        }
        
        // Reconstrução: backtracking pela dimensão dos itens
        int *selected = malloc(m * sizeof(int)); // índices globais dos produtos escolhidos
        int countSelected = 0;
        int i = m, w = W, v = V;
        while (i > 0) {
            if (dec[IDX(i, w, v)] == 1) {
                int idxProd = avail[i - 1];
                selected[countSelected++] = idxProd;
                w -= products[idxProd].weight;
                v -= products[idxProd].volume;
                i--;
            } else {
                i--;
            }
        }
        
        // Ordena os produtos selecionados conforme a ordem de entrada
        if (countSelected > 0)
            qsort(selected, countSelected, sizeof(int), cmp_int);
        
        // Acumula os totais e marca os produtos selecionados como usados
        double totalValue = 0.0;
        int totalWeight = 0;
        int totalVolume = 0;
        for (int k = 0; k < countSelected; k++) {
            int idxProd = selected[k];
            products[idxProd].used = 1;
            totalValue += products[idxProd].value;
            totalWeight += products[idxProd].weight;
            totalVolume += products[idxProd].volume;
        }
        
        int percWeight = (int) round(totalWeight * 100.0 / trucks[t].max_weight);
        int percVolume = (int) round(totalVolume * 100.0 / trucks[t].max_volume);
        
        // Escreve a linha do caminhão
        fprintf(fout, "[%s]R$%.2f,%dKG(%d%%),%dL(%d%%)->", trucks[t].plate, totalValue, totalWeight, percWeight, totalVolume, percVolume);
        for (int k = 0; k < countSelected; k++) {
            fprintf(fout, "%s", products[selected[k]].code);
            if (k < countSelected - 1)
                fprintf(fout, ",");
        }
        fprintf(fout, "\n");
        
        // Libera memória alocada para este caminhão
        free(dp);
        free(dec);
        free(avail);
        free(selected);
        
        #undef IDX
    }
    
    // Lista os produtos pendentes (não alocados)
    int pendingExists = 0;
    for (int i = 0; i < numProducts; i++) {
        if (!products[i].used) {
            pendingExists = 1;
            break;
        }
    }
    if (pendingExists) {
        fprintf(fout, "PENDENTE:");
        double valorPendente = 0;
        int pesoPendente = 0;
        int volPendente = 0;
        for (int i = 0; i < numProducts; i++) {
            if (!products[i].used) {
                valorPendente += products[i].value;
                pesoPendente += products[i].weight;
                volPendente += products[i].volume;
            }
        }
        fprintf(fout, "R$%.2f,%dKG,%dL->", valorPendente, pesoPendente, volPendente);
        
        int first = 1; 
        for (int i = 0; i < numProducts; i++) {
            if (!products[i].used) {
                if (first) {
                    fprintf(fout, "%s", products[i].code); 
                    first = 0;
                } else {
                    fprintf(fout, ",%s", products[i].code);
                }
            }
        }
    }
    
    free(trucks);
    free(products);
    fclose(fin);
    fclose(fout);
    
    return 0;
}
