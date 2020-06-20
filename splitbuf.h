#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include "array.h"

typedef struct SplitBuffer {
  Array* preGap;
  Array* postGap;
} SplitBuffer;

void splitbuf_init(SplitBuffer *b, size_t init);
void splitbuf_destroy(SplitBuffer *b);
void splitbuf_insert(SplitBuffer *b, int c);
void splitbuf_inserts(SplitBuffer *b, const char *s);
void splitbuf_forward(SplitBuffer *b); // move right
void splitbuf_backward(SplitBuffer *b); // move left
void splitbuf_move(SplitBuffer *b, ptrdiff_t amt);
void splitbuf_backspace(SplitBuffer *b); // delete before cursor
void splitbuf_delete(SplitBuffer *b); // delete after cursor
