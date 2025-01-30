#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

void swap(int32_t arr[], uint32_t i, uint32_t j) {
    int32_t temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
}

void heapify(int32_t* arr, uint32_t n, uint32_t curr_index) {
    uint32_t root_index = curr_index;
    uint32_t left_child_index = 2 * curr_index + 1;
    uint32_t right_child_index = 2 * curr_index + 2;

    if (left_child_index < n && 
        arr[left_child_index] > arr[root_index])
    {
        root_index = left_child_index;    
    }

    if (right_child_index < n &&
        arr[right_child_index] > arr[root_index]) 
    {
        root_index = right_child_index;
    }

    if (root_index != curr_index) {
        swap(arr, root_index, curr_index);
        heapify(arr, n, root_index);
    }
}

void heapsort(int32_t arr[], uint32_t n) {
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

int main() {
    int32_t arr[] = {50, 23, 98, 12, 76, 34, 65, 87, 14, 3,
    91, 45, 67, 29, 81, 55, 22, 7, 40, 62,
    37, 84, 19, 73, 10, 99, 30, 1, 56, 88,
    16, 92, 48, 5, 68, 25, 31, 79, 41, 60,
    8, 96, 47, 33, 77, 9, 100, 6, 85, 20};
    uint32_t n = sizeof(arr) / sizeof(arr[0]);

    printf("Original arr:\n");
    for (int32_t i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }

    heapsort(arr, n);

    printf("\n");
    printf("Ordered arr:\n");
    for (int32_t i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }


    return 0;
}