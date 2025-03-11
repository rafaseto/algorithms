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

// Funções para alocar e liberar arrays 3D de double e int
double ***alloc_3d_double(int d1, int d2, int d3) {
    double ***arr = malloc(d1 * sizeof(double **));
    for (int i = 0; i < d1; i++) {
        arr[i] = malloc(d2 * sizeof(double *));
        for (int j = 0; j < d2; j++) {
            arr[i][j] = malloc(d3 * sizeof(double));
            for (int k = 0; k < d3; k++) {
                arr[i][j][k] = 0.0;
            }
        }
    }
    return arr;
}

void free_3d_double(double ***arr, int d1, int d2) {
    for (int i = 0; i < d1; i++) {
        for (int j = 0; j < d2; j++) {
            free(arr[i][j]);
        }
        free(arr[i]);
    }
    free(arr);
}

int ***alloc_3d_int(int d1, int d2, int d3) {
    int ***arr = malloc(d1 * sizeof(int **));
    for (int i = 0; i < d1; i++) {
        arr[i] = malloc(d2 * sizeof(int *));
        for (int j = 0; j < d2; j++) {
            arr[i][j] = malloc(d3 * sizeof(int));
            for (int k = 0; k < d3; k++) {
                arr[i][j][k] = 0;
            }
        }
    }
    return arr;
}

void free_3d_int(int ***arr, int d1, int d2) {
    for (int i = 0; i < d1; i++) {
        for (int j = 0; j < d2; j++) {
            free(arr[i][j]);
        }
        free(arr[i]);
    }
    free(arr);
}

// Função para ordenar índices de produtos de acordo com a ordem de entrada
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
    Truck *trucks = (Truck *) malloc(numTrucks * sizeof(Truck));
    for (int i = 0; i < numTrucks; i++) {
        fscanf(fin, "%s %d %d", trucks[i].plate, &trucks[i].max_weight, &trucks[i].max_volume);
    }
    
    int numProducts;
    fscanf(fin, "%d", &numProducts);
    Product *products = (Product *) malloc(numProducts * sizeof(Product));
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
        
        // Aloca a tabela de DP (dimensões: (m+1) x (W+1) x (V+1)) e a tabela de decisões
        double ***dp = alloc_3d_double(m + 1, W + 1, V + 1);
        int ***dec = alloc_3d_int(m + 1, W + 1, V + 1);
        
        // DP: para i de 1 a m (itens disponíveis)
        for (int i = 1; i <= m; i++) {
            int idx = avail[i - 1]; // índice global do produto
            int pweight = products[idx].weight;
            int pvolume = products[idx].volume;
            double pvalue = products[idx].value;
            for (int w = 0; w <= W; w++) {
                for (int v = 0; v <= V; v++) {
                    // Opção: não usar o produto i
                    dp[i][w][v] = dp[i - 1][w][v];
                    dec[i][w][v] = 0;
                    // Se couber o produto, tenta incluí-lo
                    if (w >= pweight && v >= pvolume) {
                        double candidate = dp[i - 1][w - pweight][v - pvolume] + pvalue;
                        if (candidate > dp[i][w][v]) {
                            dp[i][w][v] = candidate;
                            dec[i][w][v] = 1;
                        }
                    }
                }
            }
        }
        
        // Reconstrução: backtracking pela dimensão dos itens
        int *selected = malloc(m * sizeof(int)); // guardará os índices globais dos produtos escolhidos
        int countSelected = 0;
        int i = m, w = W, v = V;
        while (i > 0) {
            if (dec[i][w][v] == 1) {
                int idx = avail[i - 1];
                selected[countSelected++] = idx;
                w -= products[idx].weight;
                v -= products[idx].volume;
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
            int idx = selected[k];
            products[idx].used = 1;
            totalValue += products[idx].value;
            totalWeight += products[idx].weight;
            totalVolume += products[idx].volume;
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
        
        // Libera as estruturas alocadas para este caminhão
        free_3d_double(dp, m + 1, W + 1);
        free_3d_int(dec, m + 1, W + 1);
        free(avail);
        free(selected);
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
        int first = 1;
        for (int i = 0; i < numProducts; i++) {
            if (!products[i].used) {
                if (!first)
                    fprintf(fout, ",");
                fprintf(fout, "R$%.2f,%dKG,%dL->%s", products[i].value, products[i].weight, products[i].volume, products[i].code);
                first = 0;
            }
        }
        fprintf(fout, "\n");
    }
    
    free(trucks);
    free(products);
    fclose(fin);
    fclose(fout);
    
    return 0;
}
