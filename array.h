typedef struct Array {
  char* items;
  int capacity; // how many elements it can hold
  int size; // how many elements are currently inside
} Array;

void array_init(Array* arr, int initial_capacity);
void array_append(Array* arr, int item);
int array_remove_last(Array* arr);
void array_destroy(Array* arr);

