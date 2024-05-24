#include <stdio.h>
#include <stdlib.h>
#include "dynamic_array.h"

void initArray(DynamicArray *arr, int initialCapacity) {
    arr->data = (int *)malloc(initialCapacity * sizeof(int));
    if (arr->data == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    arr->size = 0;
    arr->capacity = initialCapacity;
}

void addElement(DynamicArray *arr, int element) {
    if (arr->size == arr->capacity) {
        arr->capacity *= 2;
        int *temp = (int *)realloc(arr->data, arr->capacity * sizeof(int));
        if (temp == NULL) {
            free(arr->data);
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }
        arr->data = temp;
    }
    arr->data[arr->size++] = element;
}

void printArray(const DynamicArray *arr) {
    printf("size: %d\n", arr->size);
    for (int i = 0; i < arr->size; i++) {
        printf("%d ", arr->data[i]);
    }
    printf("\n");
}

void freeArray(DynamicArray *arr) {
    free(arr->data);
    arr->data = NULL;
    arr->size = 0;
    arr->capacity = 0;
}

int example() {
    DynamicArray arr;
    initArray(&arr, 10); 
    printArray(&arr);
    
    for (int i = 0; i < 20; i++) {
        addElement(&arr, i);
    }

    printArray(&arr);

    freeArray(&arr);

    return 0;
}