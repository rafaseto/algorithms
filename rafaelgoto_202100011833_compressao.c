#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdint.h>

typedef struct Node {
    unsigned char symbol;
    unsigned int freq;
    struct Node *left, *right;
} Node;

static inline void swap(Node **a, Node **b) {
    Node *temp = *a;
    *a = *b;
    *b = temp;
}

void heapify(Node **heap, int size, int i) {
    int smallest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    if (left < size && heap[left]->freq < heap[smallest]->freq)
        smallest = left;
    if (right < size && heap[right]->freq < heap[smallest]->freq)
        smallest = right;
    if (smallest != i) {
        swap(&heap[i], &heap[smallest]);
        heapify(heap, size, smallest);
    }
}

void insertHeap(Node **heap, int *size, Node *node) 
{
    int i = *size;
    heap[i] = node;
    (*size)++;
    while (i != 0 && heap[(i - 1) / 2]->freq > heap[i]->freq) {
        swap(&heap[i], &heap[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

Node* extractMin(Node **heap, int *size) {
    Node* min = heap[0];
    heap[0] = heap[*size - 1];
    (*size)--;
    heapify(heap, *size, 0);
    return min;
}

Node* buildHuffmanTree(unsigned int freq[256]) {
    Node **heap = (Node**)malloc(256 * sizeof(Node*));
    int size = 0;
    for (int i = 0; i < 256; i++) 
    {
        if (freq[i] > 0) {
            // printf("Entrou na construcao da arvore");
            Node *node = (Node*)malloc(sizeof(Node));
            node->symbol = (unsigned char)i;
            node->freq = freq[i];
            node->left = node->right = NULL;
            insertHeap(heap, &size, node);
        }
    }
    if (size == 0) {
        free(heap);
        return NULL;
    }
    if (size == 1) {
        Node *only = extractMin(heap, &size);
        Node *root = (Node*)malloc(sizeof(Node));
        root->freq = only->freq;
        root->left = only;
        root->right = NULL;
        free(heap);
        return root;
    }
    while (size > 1) 
    {
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

void freeTree(Node *root) {
    if (root == NULL) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

typedef struct {
    uint64_t code;
    int len;
} CodeInfo;

typedef struct StackNode {
    Node *node;
    uint64_t code;
    int len;
} StackNode;

void assignCodes(Node *root, CodeInfo *code_info) {
    if (!root) return;

    StackNode stack[512];
    int top = -1;
    stack[++top] = (StackNode){root, 0, 0};

    while (top >= 0) {
        // printf("Entrou no while do assign");



        StackNode current = stack[top--];
        Node *node = current.node;
        uint64_t code = current.code;
        int len = current.len;

        if (!node->left && !node->right) {
            if (len == 0) {
                code_info[node->symbol].code = 0;
                code_info[node->symbol].len = 1;
            } else {
                code_info[node->symbol].code = code;
                code_info[node->symbol].len = len;
                // printf("Codigo entrou no segundo caso do if 07")
            }
        } else {
            if (node->right) {
                stack[++top] = (StackNode){node->right, (code << 1) | 1, len + 1};
            }
            if (node->left) {
                stack[++top] = (StackNode){node->left, (code << 1) | 0, len + 1};
            }
        }
    }
}

unsigned char* huffmanEncode(unsigned char *seq, int n, int *out_len) {
    unsigned int freq[256] = {0};
    for (int i = 0; i < n; i++) 
    {
        freq[seq[i]]++;
    }
    Node *root = buildHuffmanTree(freq);
    CodeInfo code_info[256] = {{0}};
    if (root) {
        if (root->left || root->right) {
            assignCodes(root, code_info);
        } else {
            code_info[root->symbol].code = 0;
            code_info[root->symbol].len = 1;

            // printf("Entrou no HUF encode (else)");
        }
    }

    int total_bits = 0;
    for (int i = 0; i < n; i++) {
        total_bits += code_info[seq[i]].len;
    }
    int num_bytes = (total_bits + 7) / 8;
    unsigned char *out = (unsigned char*)malloc(num_bytes);
    memset(out, 0, num_bytes);

    uint64_t bit_buffer = 0;
    int bit_count = 0;
    int byte_index = 0;

    for (int i = 0; i < n; i++) 
    {
        unsigned char symbol = seq[i];
        uint64_t code = code_info[symbol].code;


        int len = code_info[symbol].len;

        if (len == 0) continue;

        uint64_t shifted_code = code << (64 - len);
        bit_buffer |= shifted_code >> bit_count;
        bit_count += len;

        while (bit_count >= 8) {
            out[byte_index++] = (bit_buffer >> (64 - 8)) & 0xFF;
            bit_buffer <<= 8;
            bit_count -= 8;
        }
    }

    if (bit_count > 0) {
        out[byte_index++] = (bit_buffer >> (64 - 8)) & 0xFF;
    }

    *out_len = byte_index;
    freeTree(root);
    return out;
}

unsigned char* RLEEncode(unsigned char *seq, int n, int *out_len) {
    unsigned char *out = (unsigned char*)malloc(2 * n);
    int j = 0;
    int i = 0;
    while (i < n) {
        int count = 1;
        while (i + count < n && seq[i + count] == seq[i] && count < 255) 
        {
            count++;
        }
        out[j++] = (unsigned char) count;
        out[j++] = seq[i];
        i += count;

        // printf("entra no RLE encode (while 2)")
    }
    *out_len = j;
    return out;
}

char* bytesToHexString(unsigned char *bytes, int len) 
{
    char *hex = (char*)malloc(2 * len + 1);
    static const char hex_table[] = "0123456789ABCDEF";
    for (int i = 0; i < len; i++) {
        unsigned char byte = bytes[i];
        hex[2*i] = hex_table[byte >> 4];
        hex[2*i + 1] = hex_table[byte & 0x0F];
    }
    hex[2*len] = '\0';
    return hex;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s entrada.txt saida.txt\n", argv[0]);
        return 1;
    }

    FILE *fin = fopen(argv[1], "r");
    if (!fin) {
        fprintf(stderr, "Erro ao abrir o arquivo de entrada.\n");
        return 1;
    }

    fseek(fin, 0, SEEK_END);
    long fsize = ftell(fin);
    fseek(fin, 0, SEEK_SET);
    char *buffer = (char*)malloc(fsize + 1);
    fread(buffer, 1, fsize, fin);
    fclose(fin);
    buffer[fsize] = '\0';

    FILE *fout = fopen(argv[2], "w");
    if (!fout) {
        fprintf(stderr, "Erro ao abrir o arquivo de saída.\n");
        free(buffer);
        return 1;
    }

    char *ptr = buffer;
    int numSeq = strtol(ptr, &ptr, 10);

    for (int seqIndex = 0; seqIndex < numSeq; seqIndex++) {
        while (*ptr && (*ptr == ' ' || *ptr == '\n' || *ptr == '\t')) ptr++;
        int count = strtol(ptr, &ptr, 10);

        unsigned char *seq = (unsigned char*)malloc(count);
        for (int i = 0; i < count; i++) {
            while (*ptr && (*ptr == ' ' || *ptr == '\n' || *ptr == '\t')) ptr++;
            unsigned int val = strtol(ptr, &ptr, 16);
            seq[i] = (unsigned char)val;
        }

        int huf_out_len;
        unsigned char *hufEncoded = huffmanEncode(seq, count, &huf_out_len);
        char *hufHex = bytesToHexString(hufEncoded, huf_out_len);
        double huf_ratio = huf_out_len * 100.0 / count;

        int rle_out_len;
        unsigned char *rleEncoded = RLEEncode(seq, count, &rle_out_len);
        char *rleHex = bytesToHexString(rleEncoded, rle_out_len);
        double rle_ratio = rle_out_len * 100.0 / count;

        double tol = 1e-6;
        if (fabs(huf_ratio - rle_ratio) < tol) 
        {
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

    free(buffer);
    fclose(fout);
    return 0;
}