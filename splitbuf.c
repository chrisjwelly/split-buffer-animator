#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "splitbuf.h"

void splitbuf_init(SplitBuffer* b, size_t init) {
  Array* preGap = (Array*)malloc(sizeof(Array));
  Array* postGap = (Array*)malloc(sizeof(Array));
  array_init(preGap, init);
  array_init(postGap, init);

  b->preGap = preGap;
  b->postGap = postGap;
}

void splitbuf_destroy(SplitBuffer* b) {
  array_destroy(b->preGap);
  array_destroy(b->postGap);
}

void splitbuf_insert(SplitBuffer* b, int c) {
  array_append(b->preGap, c);
}

void splitbuf_inserts(SplitBuffer* b, const char *s) {
  size_t len = strlen(s);
  for (int i = 0; i < len; i++) {
    splitbuf_insert(b, s[i]);
  }
  // By right, can just copy immediately from s using memcpy, I think.
  // but due to the implementation of dynamic array_append, it might
  // not be wise to do so.
  // Unless we write a new method to append a string to the array
}

void splitbuf_forward(SplitBuffer* b) {
  if (b->preGap->size == 0 && b->postGap->size == 0) {
    // both empty; nothing to do here
    return;
  }

  if (!(b->postGap->size == 0)) {
    int removed = array_remove_last(b->postGap);
    array_append(b->preGap, removed);
  }
}

void splitbuf_backward(SplitBuffer* b) {
  if (b->preGap->size == 0 && b->postGap->size == 0) {
    // both empty; nothing to do here
    return;
  }

  if (!(b->preGap->size == 0)) {
    int removed = array_remove_last(b->preGap);
    array_append(b->postGap, removed);
  }
}


void splitbuf_move(SplitBuffer *b, ptrdiff_t amt) {
  size_t len;
  if (amt < 0) {
    len = -amt;
    for (int i = 0; i < len; i++) {
      splitbuf_backward(b);
    }
  } else {
    len = amt;
    for (int i = 0; i < len; i++) {
      splitbuf_forward(b);
    }
  }
}

void splitbuf_backspace(SplitBuffer* b) {
  if (!(b->preGap->size == 0)) {
    array_remove_last(b->preGap);
  }
}

void splitbuf_delete(SplitBuffer* b) {
  if (!(b->postGap->size == 0)) {
    array_remove_last(b->postGap);
  }
}

// Displays the sequence
void splitbuf_flush(SplitBuffer *b) {
  printf("Flushing...\n");
  printf("Size of preGap: %d, postGap: %d\n", b->preGap->size, b->postGap->size);
  for (int i = 0; i < b->preGap->size; i++) {
    printf("%c ", b->preGap->items[i]);
  }

  printf("| ");

  for (int i = b->postGap->size - 1; i >= 0; i--) {
    printf("%c ", b->postGap->items[i]);
  }

  printf("\nEnd of flushing...\n");
}

// ---START OF TESTING METHODS---
void general_test() {
  SplitBuffer b;
  splitbuf_init(&b, 2);

  splitbuf_insert(&b, '1');
  splitbuf_insert(&b, '3');
  splitbuf_flush(&b); // 1 3 | 

  splitbuf_backward(&b); // 1 | 3
  splitbuf_flush(&b);

  splitbuf_insert(&b, '2'); // 1 2 | 3
  splitbuf_flush(&b);

  splitbuf_insert(&b, '8'); // 1 2 8 | 3
  splitbuf_flush(&b);

  splitbuf_forward(&b);
  splitbuf_flush(&b);

  splitbuf_insert(&b, '9'); // 1 2 8 3 9 |
  splitbuf_flush(&b);

  splitbuf_backspace(&b); // 1 2 8 3 |
  splitbuf_flush(&b);

  splitbuf_backward(&b);
  splitbuf_backward(&b);
  splitbuf_backward(&b); // 1 | 2 8 3
  splitbuf_backspace(&b); // | 2 8 3
  splitbuf_flush(&b);

  splitbuf_destroy(&b);
}

void backspaces_on_empty_text() {
  SplitBuffer b;
  splitbuf_init(&b, 2);

  splitbuf_flush(&b); // |
  splitbuf_backspace(&b);
  splitbuf_backspace(&b);
  splitbuf_backspace(&b);
  splitbuf_backspace(&b);
  splitbuf_backspace(&b);
  splitbuf_flush(&b); // |

  splitbuf_destroy(&b);
}

void deletes_on_empty_text() {
  SplitBuffer b;
  splitbuf_init(&b, 2);

  splitbuf_flush(&b); // |
  splitbuf_delete(&b);
  splitbuf_delete(&b);
  splitbuf_delete(&b);
  splitbuf_delete(&b);
  splitbuf_delete(&b);
  splitbuf_flush(&b); // |

  splitbuf_destroy(&b);
}

void forwards_on_empty_text() {
  SplitBuffer b;
  splitbuf_init(&b, 2);

  splitbuf_flush(&b); // |
  splitbuf_forward(&b);
  splitbuf_forward(&b);
  splitbuf_forward(&b);
  splitbuf_forward(&b);
  splitbuf_flush(&b); // |
  
  splitbuf_destroy(&b);
}

void backwards_on_empty_text() {
  SplitBuffer b;
  splitbuf_init(&b, 2);

  splitbuf_flush(&b); // |
  splitbuf_backward(&b);
  splitbuf_backward(&b);
  splitbuf_backward(&b);
  splitbuf_backward(&b);
  splitbuf_flush(&b); // |

  splitbuf_destroy(&b);
}

void multiple_backwards_then_forwards() {
  SplitBuffer b;
  splitbuf_init(&b, 2);

  splitbuf_insert(&b, '1');
  splitbuf_insert(&b, '2');
  splitbuf_insert(&b, '3');
  splitbuf_insert(&b, '4');
  splitbuf_insert(&b, '5');
  splitbuf_insert(&b, '6');
  splitbuf_insert(&b, '7');
  splitbuf_insert(&b, '8');
  splitbuf_flush(&b); // 1 2 3 4 ... 8 |

  splitbuf_backward(&b);
  splitbuf_flush(&b); // 1 2 3 4 ... 7 | 8

  splitbuf_backward(&b);
  splitbuf_backward(&b);
  splitbuf_backward(&b);
  splitbuf_backward(&b);
  splitbuf_flush(&b); // 1 2 3 | 4 5 6 .. 8

  splitbuf_backward(&b);
  splitbuf_backward(&b);
  splitbuf_backward(&b);
  splitbuf_flush(&b); // | 1 2 3 ... 7 8 

  splitbuf_backward(&b); // no effect
  splitbuf_forward(&b);
  splitbuf_flush(&b); // 1 | 2 3 ... 7 8
  
  splitbuf_forward(&b);
  splitbuf_forward(&b);
  splitbuf_forward(&b);
  splitbuf_forward(&b);
  splitbuf_forward(&b);
  splitbuf_flush(&b); // 1 2 3 ... 6 | 7 8

  splitbuf_forward(&b);
  splitbuf_forward(&b);
  splitbuf_flush(&b); // 1 2 3 ... 8 |

  splitbuf_forward(&b); // no effect
  splitbuf_backward(&b);
  splitbuf_flush(&b); // 1 2 3 ... 7 | 8

  splitbuf_destroy(&b);
}

void batch_inserts() {
  SplitBuffer b;
  splitbuf_init(&b, 2);

  splitbuf_inserts(&b, "abc");
  splitbuf_flush(&b); // abc|

  splitbuf_inserts(&b, "ef");
  splitbuf_flush(&b); // abcef|

  splitbuf_backward(&b);
  splitbuf_backward(&b);
  splitbuf_inserts(&b, "d");
  splitbuf_flush(&b); // abcdef|

  splitbuf_destroy(&b);
}

void test_move() {
  SplitBuffer b;
  splitbuf_init(&b, 2);

  splitbuf_inserts(&b, "SplitBuffer!");
  splitbuf_flush(&b); // SplitBuffer!|

  splitbuf_move(&b, -12);
  splitbuf_flush(&b); //|SplitBuffer!

  splitbuf_move(&b, 5);
  splitbuf_flush(&b); // Split|Buffer!

  splitbuf_inserts(&b, " sploot ");
  splitbuf_flush(&b); // Split sploot |Buffer!

  splitbuf_destroy(&b);
}

void backspaces_and_deletes() {
  SplitBuffer b;
  splitbuf_init(&b, 2);

  splitbuf_inserts(&b, "fdbaceg");
  splitbuf_flush(&b); // fdbaceg|

  splitbuf_move(&b, -4);
  splitbuf_flush(&b); // fdb|aceg

  splitbuf_delete(&b);
  splitbuf_flush(&b); // fdb|ceg

  splitbuf_backspace(&b);
  splitbuf_flush(&b); // fd|ceg

  splitbuf_delete(&b);
  splitbuf_backspace(&b);
  splitbuf_flush(&b); // f|eg

  splitbuf_delete(&b);
  splitbuf_backspace(&b);
  splitbuf_delete(&b);
  splitbuf_flush(&b); // |

  splitbuf_destroy(&b);
}

void run_tests() {
  printf("general_test():\n");
  general_test();
  printf("\n");

  printf("backspaces_on_empty_text():\n");
  backspaces_on_empty_text();
  printf("\n");

  printf("deletes_on_empty_text():\n");
  deletes_on_empty_text();
  printf("\n");

  printf("forwards_on_empty_text():\n");
  forwards_on_empty_text();
  printf("\n");

  printf("backwards_on_empty_text():\n");
  backwards_on_empty_text();
  printf("\n");

  printf("multiple_backwards_then_forwards():\n");
  multiple_backwards_then_forwards();
  printf("\n");

  printf("batch_inserts():\n");
  batch_inserts();
  printf("\n");

  printf("test_move():\n");
  test_move();
  printf("\n");

  printf("backspaces_and_deletes():\n");
  backspaces_and_deletes();
  printf("\n");
}
// ---END OF TESTING METHODS---

int main(void) {
  run_tests();
  return 0;
}
