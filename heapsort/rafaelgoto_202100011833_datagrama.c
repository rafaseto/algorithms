#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    int32_t number;
    unsigned char data[512];
    int32_t size;
} Packet;

typedef struct {
    int32_t number;
    unsigned char data[10];
    int32_t size;
} PacketTest02;


void swap(Packet** a, Packet** b) {
    Packet* temp = *a;
    *a = *b;
    *b = temp;
}

void pacotesTeste01(Packet packet) {
    printf("%d", packet.number);
}

void pacotesTeste02(Packet packet) {
    printf("%d", packet.size);
}

void pacotesTeste03(Packet packet) {
    for(int i=0; i<10; i++) {
        printf("%d", packet.data[i]);
    }
}

void heapify(Packet** arr, uint32_t n, uint32_t curr_index) {
    while (1) 
    {
        uint32_t root_index = curr_index;
        uint32_t left_child_index = 2 * curr_index + 1;
        uint32_t right_child_index = 2 * curr_index + 2;

        if (left_child_index < n && arr[left_child_index]->number > arr[root_index]->number) {
            root_index = left_child_index;
        }
        if (right_child_index < n && arr[right_child_index]->number > arr[root_index]->number) {
            root_index = right_child_index;
        }
        if (root_index == curr_index) break;

        swap(&arr[root_index], &arr[curr_index]);
        curr_index = root_index;
    }
}

void heapsort(Packet** arr, uint32_t n) {
    for (int32_t i = n / 2 - 1; i >= 0; i--) {
        heapify(arr, n, i);
    }
    
    for (int32_t i = n - 1; i > 0; i--) {
        swap(&arr[0], &arr[i]);
        heapify(arr, i, 0);
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
    
    int32_t num_packets, read_interval;
    fscanf(input, "%d %d", &num_packets, &read_interval);
    PacketTest02 packetsTest;
    Packet** packets = (Packet**)malloc(num_packets * sizeof(Packet*));
    uint8_t* received = (uint8_t*)calloc(num_packets, sizeof(uint8_t));
    int32_t count = 0, expected_packet = 0;

    while (num_packets > 0) {
        for (int32_t i = 0; i < read_interval && num_packets > 0; i++, num_packets--) {
            int32_t packet_number, packet_size;
            fscanf(input, "%d %d", &packet_number, &packet_size);
            
            Packet* curr_packet = (Packet*)malloc(sizeof(Packet));
            curr_packet->number = packet_number;
            curr_packet->size = packet_size;
            
            for (int j = 0; j < packet_size; j++) {
                unsigned int temp;
                if (fscanf(input, "%2x", &temp) != 1) {
                    fprintf(stderr, "Deu algum Erro ao ler os dados hexadecimais.\n");
                    return 1;
                }
                curr_packet->data[j] = (unsigned char)temp;
            }
            packets[count++] = curr_packet;
            received[packet_number] = 1;
        }

        if (count > 0 && packets[0]->number != expected_packet) {
            heapsort(packets, count);
        }

        int32_t print_index = 0;

        if (count > 0 && packets[0]->number == expected_packet) {
            fprintf(output, "|");
            while (print_index < count && packets[print_index]->number == expected_packet) {
                for (int32_t j = 0; j < packets[print_index]->size - 1; j++) {
                    fprintf(output, "%02X,", packets[print_index]->data[j]);
                }
                fprintf(output, "%02X|", packets[print_index]->data[packets[print_index]->size - 1]);
                expected_packet++;

                free(packets[print_index]);
                print_index++;
            }

            count -= print_index;
            memmove(packets, packets + print_index, count * sizeof(Packet*));
            fprintf(output, "\n");
        }
    }

    for (int i = 0; i < count; i++) {
        free(packets[i]);
    }

    free(packets);
    free(received);
    fclose(input);
    fclose(output);
    
    return 0;
}