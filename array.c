#include <stdio.h>
#include <stdlib.h>
#include "array.h"

void array_init(Array* arr, int initial_capacity) {
  arr->items = calloc(initial_capacity, sizeof(int));
  arr->capacity = initial_capacity;
  arr->size = 0;
}

void array_append(Array* arr, int item) {
  arr->items[arr->size] = item;
  arr->size++;
  if (arr->size == arr->capacity) {
    int* new_items = calloc(arr->capacity * 2, sizeof(int));
    for (int i = 0; i < arr->size; i++) {
      new_items[i] = arr->items[i];
    }
    arr->items = new_items;
    arr->capacity *= 2;
  }
}

int array_remove_last(Array* arr) {
  int deleted = arr->items[arr->size - 1];
  arr->items[arr->size - 1] = 0; // by right can also not do anything
  arr->size--;
  if (arr->size == (int) (arr->capacity / 4)) {
    int* new_items = calloc((int) (arr->capacity / 2), sizeof(int));
    for (int i = 0; i < arr->size; i++) {
      new_items[i] = arr->items[i];
    }
    arr->items = new_items;
    arr->capacity = (int) (arr->capacity / 2);
  }
  return deleted;
}

void array_destroy(Array* arr) {
  free(arr->items);
}

/*
int main() {
  Array arr;
  array_init(&arr, 4);
  printf("Initial capacity: %d\n", arr.capacity);

  printf("---Appends---\n");
  for (int i = 0; i < 16; i++) {
    array_append(&arr, i);
    printf("%d, Size: %d, Capacity: %d\n", arr.items[i], arr.size, arr.capacity);
  }
  printf("---Removes---\n");
  for (int i = 0; i < 14; i++) {
    array_remove_last(&arr);
    printf("Size: %d, Capacity: %d\n", arr.size, arr.capacity);
  }

  printf("append remove append remove\n");
  array_append(&arr, 100);
  printf("Size: %d, Capacity: %d\n", arr.size, arr.capacity);
  array_remove_last(&arr);
  printf("Size: %d, Capacity: %d\n", arr.size, arr.capacity);
  array_append(&arr, 100);
  printf("Size: %d, Capacity: %d\n", arr.size, arr.capacity);
  array_remove_last(&arr);
  printf("Size: %d, Capacity: %d\n", arr.size, arr.capacity);

  array_destroy(&arr);
  return 0;

}
*/
