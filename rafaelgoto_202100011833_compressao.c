#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Estrutura para os nós da árvore de Huffman */
typedef struct Node {
    unsigned char symbol;
    unsigned int freq;
    struct Node *left, *right;
} Node;

/* Função para trocar dois ponteiros para Node */
void swap(Node **a, Node **b) {
    Node *temp = *a;
    *a = *b;
    *b = temp;
}

/* Função para manter a propriedade de min-heap */
void heapify(Node **heap, int size, int i) {
    int smallest = i;
    int left = 2*i + 1;
    int right = 2*i + 2;
    if (left < size && heap[left]->freq < heap[smallest]->freq)
         smallest = left;
    if (right < size && heap[right]->freq < heap[smallest]->freq)
         smallest = right;
    if (smallest != i) {
         swap(&heap[i], &heap[smallest]);
         heapify(heap, size, smallest);
    }
}

/* Insere um nó na heap */
void insertHeap(Node **heap, int *size, Node *node) {
    int i = *size;
    heap[i] = node;
    (*size)++;
    while (i != 0 && heap[(i - 1) / 2]->freq > heap[i]->freq) {
         swap(&heap[i], &heap[(i - 1) / 2]);
         i = (i - 1) / 2;
    }
}

/* Extrai o nó de menor frequência da heap */
Node* extractMin(Node **heap, int *size) {
    Node* min = heap[0];
    heap[0] = heap[*size - 1];
    (*size)--;
    heapify(heap, *size, 0);
    return min;
}

/* Constrói a árvore de Huffman usando uma heap mínima.
   Para cada símbolo com frequência > 0, cria um nó folha e insere na heap.
   Em seguida, combina os dois nós de menor frequência até sobrar um único nó. */
Node* buildHuffmanTree(unsigned int freq[256]) {
    Node **heap = (Node**)malloc(256 * sizeof(Node*));
    int size = 0;
    for (int i = 0; i < 256; i++){
       if (freq[i] > 0) {
          Node *node = (Node*)malloc(sizeof(Node));
          node->symbol = (unsigned char)i;
          node->freq = freq[i];
          node->left = node->right = NULL;
          insertHeap(heap, &size, node);
       }
    }
    if (size == 0) {  // caso não haja símbolos (não ocorrerá neste problema)
       free(heap);
       return NULL;
    }
    if (size == 1) {
       // Se houver apenas um símbolo, cria um nó pai para garantir que o código tenha pelo menos 1 bit.
       Node *only = extractMin(heap, &size);
       Node *root = (Node*)malloc(sizeof(Node));
       root->freq = only->freq;
       root->left = only;
       root->right = NULL;
       free(heap);
       return root;
    }
    while (size > 1) {
       Node *left = extractMin(heap, &size);
       Node *right = extractMin(heap, &size);
       Node *parent = (Node*)malloc(sizeof(Node));
       parent->freq = left->freq + right->freq;
       parent->left = left;
       parent->right = right;
       insertHeap(heap, &size, parent);
    }
    Node *root = extractMin(heap, &size);
    free(heap);
    return root;
}

/* Libera a memória alocada para a árvore de Huffman */
void freeTree(Node *root) {
    if (root == NULL) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

/* Atribui os códigos (como cadeia de '0' e '1') a cada símbolo, percorrendo a árvore.
   Se a árvore tiver somente um nó (único símbolo), o código atribuído é "0". */
void assignCodes(Node *root, char *code, int depth, char codes[256][256]) {
    if (!root->left && !root->right) { // nó folha
         if (depth == 0) { // caso de apenas um símbolo
             code[0] = '0';
             code[1] = '\0';
         } else {
             code[depth] = '\0';
         }
         strcpy(codes[root->symbol], code);
         return;
    }
    if (root->left) {
         code[depth] = '0';
         assignCodes(root->left, code, depth + 1, codes);
    }
    if (root->right) {
         code[depth] = '1';
         assignCodes(root->right, code, depth + 1, codes);
    }
}

/* Realiza a codificação por Huffman.
   – Calcula as frequências dos símbolos na sequência.
   – Constrói a árvore de Huffman e atribui os códigos.
   – Gera uma cadeia de bits (como uma string de '0' e '1'),
     empacota esses bits em bytes (com preenchimento à direita se necessário)
     e retorna o vetor de bytes resultante e seu tamanho. */
unsigned char* huffmanEncode(unsigned char *seq, int n, int *out_len) {
    unsigned int freq[256] = {0};
    for (int i = 0; i < n; i++) {
         freq[seq[i]]++;
    }
    Node *root = buildHuffmanTree(freq);
    char codes[256][256] = {{0}};
    char code[256];
    assignCodes(root, code, 0, codes);
    // Calcula o número total de bits da codificação
    int total_bits = 0;
    for (int i = 0; i < n; i++) {
         total_bits += strlen(codes[seq[i]]);
    }
    char *bitString = (char*)malloc(total_bits + 1);
    bitString[0] = '\0';
    for (int i = 0; i < n; i++) {
         strcat(bitString, codes[seq[i]]);
    }
    // Empacota os bits em bytes
    int num_bytes = (total_bits + 7) / 8;
    unsigned char *out = (unsigned char*)malloc(num_bytes);
    memset(out, 0, num_bytes);
    for (int i = 0; i < total_bits; i++) {
         if (bitString[i] == '1') {
             out[i/8] |= (1 << (7 - (i % 8)));
         }
    }
    *out_len = num_bytes;
    free(bitString);
    freeTree(root);
    return out;
}

/* Realiza a codificação por Run-Length Encoding (RLE).
   Para cada sequência de bytes iguais consecutivos, armazena:
      [contagem (1 byte)][símbolo]
   Se a contagem ultrapassar 255, divide em blocos. */
unsigned char* RLEEncode(unsigned char *seq, int n, int *out_len) {
   unsigned char *out = (unsigned char*)malloc(2 * n); // pior caso
   int j = 0;
   int i = 0;
   while(i < n) {
      int count = 1;
      while(i + count < n && seq[i + count] == seq[i] && count < 255) {
          count++;
      }
      out[j++] = (unsigned char) count;
      out[j++] = seq[i];
      i += count;
   }
   *out_len = j;
   return out;
}

/* Converte um vetor de bytes em uma string hexadecimal (duas casas por byte, letras maiúsculas) */
char* bytesToHexString(unsigned char *bytes, int len) {
    char *hex = (char*)malloc(2 * len + 1);
    for (int i = 0; i < len; i++){
         sprintf(hex + 2*i, "%02X", bytes[i]);
    }
    hex[2 * len] = '\0';
    return hex;
}

int main(int argc, char *argv[]) {
   if (argc != 3) {
       fprintf(stderr, "Uso: %s entrada.txt saida.txt\n", argv[0]);
       return 1;
   }
   FILE *fin = fopen(argv[1], "r");  // abertura em modo somente leitura
   if (!fin) {
       fprintf(stderr, "Erro ao abrir o arquivo de entrada.\n");
       return 1;
   }
   FILE *fout = fopen(argv[2], "w"); // abertura em modo somente escrita
   if (!fout) {
       fprintf(stderr, "Erro ao abrir o arquivo de saída.\n");
       fclose(fin);
       return 1;
   }
   int numSeq;
   fscanf(fin, "%d", &numSeq);
   for (int seqIndex = 0; seqIndex < numSeq; seqIndex++) {
         int count;
         fscanf(fin, "%d", &count);
         unsigned char *seq = (unsigned char*)malloc(count);
         for (int i = 0; i < count; i++){
             unsigned int val;
             fscanf(fin, "%x", &val);
             seq[i] = (unsigned char) val;
         }
         
         /* Codificação Huffman */
         int huf_out_len;
         unsigned char *hufEncoded = huffmanEncode(seq, count, &huf_out_len);
         char *hufHex = bytesToHexString(hufEncoded, huf_out_len);
         double huf_ratio = ((double)huf_out_len / count) * 100.0;
         
         /* Codificação RLE */
         int rle_out_len;
         unsigned char *rleEncoded = RLEEncode(seq, count, &rle_out_len);
         char *rleHex = bytesToHexString(rleEncoded, rle_out_len);
         double rle_ratio = ((double)rle_out_len / count) * 100.0;
         
         /* Escolhe o método com melhor taxa (menor razão). Em caso de empate, imprime os dois,
            sendo que o Huffman é impresso primeiro. */
         double tol = 1e-6;
         if (fabs(huf_ratio - rle_ratio) < tol) {
              fprintf(fout, "%d->HUF(%.2f%%)=%s\n", seqIndex, huf_ratio, hufHex);
              fprintf(fout, "%d->RLE(%.2f%%)=%s\n", seqIndex, rle_ratio, rleHex);
         } else if (huf_ratio < rle_ratio) {
              fprintf(fout, "%d->HUF(%.2f%%)=%s\n", seqIndex, huf_ratio, hufHex);
         } else {
              fprintf(fout, "%d->RLE(%.2f%%)=%s\n", seqIndex, rle_ratio, rleHex);
         }
         
         free(seq);
         free(hufEncoded);
         free(hufHex);
         free(rleEncoded);
         free(rleHex);
   }
   fclose(fin);
   fclose(fout);
   return 0;
}
