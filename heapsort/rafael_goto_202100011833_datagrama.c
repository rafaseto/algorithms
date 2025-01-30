#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

typedef struct {
    int32_t number;
    unsigned char data[512];
    int32_t size;
} Packet;

void swap(Packet arr[], uint32_t i, uint32_t j) {
    Packet temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
}

void heapify(Packet* arr, uint32_t n, uint32_t curr_index) {
    uint32_t root_index = curr_index;
    uint32_t left_child_index = 2 * curr_index + 1;
    uint32_t right_child_index = 2 * curr_index + 2;

    if (left_child_index < n && 
        arr[left_child_index].number > arr[root_index].number)
    {
        root_index = left_child_index;    
    }

    if (right_child_index < n &&
        arr[right_child_index].number > arr[root_index].number) 
    {
        root_index = right_child_index;
    }

    if (root_index != curr_index) {
        swap(arr, root_index, curr_index);
        heapify(arr, n, root_index);
    }
}

void heapsort(Packet arr[], uint32_t n) {
    // building the heap
    for (int32_t i = n/2 - 1; i >=0; i--) {
        heapify(arr, n, i);
    }

    // sorting the arr
    for (int32_t i = n - 1; i > 0; i--) {
        swap(arr, 0, i);
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
    
    int32_t num_packets;
    int32_t read_interval;
    fscanf(input, "%d", &num_packets);
    fscanf(input, "%d", &read_interval);

    Packet packets[num_packets];
    uint32_t received[num_packets];
    int32_t count = 0;
    int32_t expected_packet = 0;

    for (int32_t i = 0; i < num_packets; i++) {
        received[i] = 0;
    }

    while (num_packets > 0) {
        for (int32_t i = 0; i < read_interval && num_packets > 0; i++, num_packets--) {
            int32_t packet_number, packet_size;
            fscanf(input, "%d", &packet_number);
            fscanf(input, "%d", &packet_size);

            Packet curr_packet;
            curr_packet.number = packet_number;
            curr_packet.size = packet_size;
            
            for (int j = 0; j < packet_size; j++) {
                unsigned int temp;
                if (fscanf(input, "%2x", &temp) != 1) {  // Lê dois caracteres hexadecimais
                    printf("Erro ao ler os dados hexadecimais.\n");
                    return 1;
                }   
                curr_packet.data[j] = (unsigned char)temp;
            }
            
            packets[count++] = curr_packet;
            received[packet_number] = 1;
        }

        heapsort(packets, count);

        int32_t print_index = 0;
        fprintf(output, "|");
        while (print_index < count && packets[print_index].number == expected_packet) {
            for (int32_t j = 0; j < packets[print_index].size - 1; j++) {
                fprintf(output, "%02X,", packets[print_index].data[j]);
            }
            fprintf(output, "%02X|",  packets[print_index].data[packets[print_index].size - 1]);

            expected_packet++;
            print_index++;
        }

        count -= print_index;
        for (int32_t i = 0; i < count; i++) {
            packets[i] = packets[i + print_index];
        }
        fprintf(output, "\n");
    }

    fclose(input);
    fclose(output);

    return 0;
}