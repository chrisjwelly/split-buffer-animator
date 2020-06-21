#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "splitbuf.h"

#define MAX(a, b) ((b) > (a) ? (b) : (a))
#define MIN(a, b) ((b) < (a) ? (b) : (a))

struct image {
    int w;
    int h;
    unsigned char rgb[];
};

struct image *
image_create(int w, int h)
{
    struct image *m = calloc(1, sizeof(*m) + (size_t)3 * w * h);
    m->w = w;
    m->h = h;
    return m;
}

struct image *
image_load(FILE *in)
{
    char c;
    int w, h;
    struct image *m = 0;
    if (fscanf(in, "P6 %d %d 255%c", &w, &h, &c) == 3 && c == '\n') {
        m = image_create(w, h);
        fread(m->rgb, h * 3, w, in);
    }
    return m;
}

void
image_set(struct image *m, int x, int y, unsigned long rgb)
{
    size_t i = (size_t)3 * m->w * y + 3 * x;
    m->rgb[i + 0] = rgb >> 16;
    m->rgb[i + 1] = rgb >>  8;
    m->rgb[i + 2] = rgb >>  0;
}

unsigned long
image_get(const struct image *m, int x, int y)
{
    size_t i = (size_t)3 * m->w * y + 3 * x;
    unsigned long r = m->rgb[i + 0];
    unsigned long g = m->rgb[i + 1];
    unsigned long b = m->rgb[i + 2];
    return (r << 16) | (g << 8) | b;
}

void
image_rect(struct image *m, int x0, int y0, int x1, int y1, unsigned long rgb)
{
    int w = abs(x1 - x0);
    int h = abs(y1 - y0);
    int bx = MIN(x0, x1);
    int by = MIN(y0, y1);
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            image_set(m, bx + x, by + y, rgb);
}

void
image_write(const struct image *m, FILE *out)
{
    fprintf(out, "P6\n%d %d\n255\n", m->w, m->h);
    fwrite(m->rgb, m->h, 3 * m->w, out);
    fflush(out);
}

#define SPLITBUF_SCALE      16
#define SPLITBUF_FONTSCALE  32
#define SPLITBUF_BG         0xffffffUL
#define SPLITBUF_FG         0x7f7f7fUL

static void
draw_block_preGap(struct image *m, int i)
{
    int x0 = i * SPLITBUF_SCALE + SPLITBUF_SCALE * 1 / 8;
    int y0 = SPLITBUF_FONTSCALE + SPLITBUF_SCALE * 1 / 8;
    int x1 = i * SPLITBUF_SCALE + SPLITBUF_SCALE * 7 / 8;
    int y1 = SPLITBUF_FONTSCALE + SPLITBUF_SCALE * 7 / 8;
    image_rect(m, x0, y0, x1, y1, SPLITBUF_FG);
}

static void
draw_block_postGap(struct image *m, int i)
{
    int x0 = i * SPLITBUF_SCALE + SPLITBUF_SCALE * 1 / 8;
    int y0 = SPLITBUF_FONTSCALE + SPLITBUF_SCALE * 1 / 8 + SPLITBUF_SCALE;
    int x1 = i * SPLITBUF_SCALE + SPLITBUF_SCALE * 7 / 8;
    int y1 = SPLITBUF_FONTSCALE + SPLITBUF_SCALE * 7 / 8 + SPLITBUF_SCALE;
    image_rect(m, x0, y0, x1, y1, SPLITBUF_FG);
}

static void
draw_char(struct image *m, int i, int c, const struct image *font, int invert)
{
    if (c < ' ' || c > '~')
        c = ' ';
    int fx = c % 16;
    int fy = c / 16 - 2;
    int fw = font->w / 16;
    int fh = font->h / 6;
    int h = SPLITBUF_FONTSCALE;
    int w = fw * h / fh;
    int bx = w * i;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float sx = fx * fw + (float)x * fw / w;
            float sy = fy * fh + (float)y * fh / h;
            unsigned long rgb = image_get(font, sx, sy);
            image_set(m, bx + x, y, invert ? -1UL ^ rgb : rgb);
        }
    }
}

static void
draw_char_preGap(struct image *m, int i, int c, const struct image *font, int invert)
{
    if (c < ' ' || c > '~')
        c = ' ';
    int fx = c % 16;
    int fy = c / 16 - 2;
    int fw = font->w / 16;
    int fh = font->h / 6;
    int h = SPLITBUF_FONTSCALE / 2;
    int w = fw * h / fh;
    int bx = w * i;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float sx = fx * fw + (float)x * fw / w;
            float sy = fy * fh + (float)y * fh / h;
            unsigned long rgb = image_get(font, sx, sy);
            image_set(m, bx + x, y + SPLITBUF_FONTSCALE, invert ? -1UL ^ rgb : rgb);
        }
    }
}

static void
draw_char_postGap(struct image *m, int i, int c, const struct image *font, int invert)
{
    if (c < ' ' || c > '~')
        c = ' ';
    int fx = c % 16;
    int fy = c / 16 - 2;
    int fw = font->w / 16;
    int fh = font->h / 6;
    int h = SPLITBUF_FONTSCALE / 2;
    int w = fw * h / fh;
    int bx = w * i;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float sx = fx * fw + (float)x * fw / w;
            float sy = fy * fh + (float)y * fh / h;
            unsigned long rgb = image_get(font, sx, sy);
            image_set(m, bx + x, y + SPLITBUF_FONTSCALE * 3/2, invert ? -1UL ^ rgb : rgb);
        }
    }
}

struct image *
splitbuf_draw(SplitBuffer *b, int w, const struct image *font)
{
    int h = SPLITBUF_FONTSCALE + SPLITBUF_SCALE + SPLITBUF_SCALE;
    struct image *m = image_create(w, h);
    image_rect(m, 0, 0, w, h, SPLITBUF_BG);

    for (size_t i = 0; i < b->preGap->size; i++) {
      draw_char_preGap(m, i, b->preGap->items[i], font, 0);
      draw_char(m, i, b->preGap->items[i], font, 0);
    }

    for (size_t i = 0; i < b->postGap->size; i++) {
      int sequenceIdx = b->preGap->size + i;
      int postGapIdx = (b->postGap->size - 1) - i;
      draw_char_postGap(m, i, b->postGap->items[i], font, 0);
      draw_char(m, sequenceIdx, b->postGap->items[postGapIdx], font, i == 0);
    }
    /* Always draw the cursor */
    if (b->postGap->size == 0)
      draw_char(m, b->preGap->size, 0, font, 1);
    return m;
}

enum opcode {
    C_HALT,
    C_WAIT,
    C_FORWARD,
    C_BACKWARD,
    C_QMOVE,
    C_INSERT,
    C_QINSERT,
    C_STRING,
    C_QSTRING,
    C_DELETE,
    C_BACKSPACE,
};

struct command {
    enum opcode op;
    union {
        char *s;
        int v;
    } arg;
};

#define FRAME() \
    do { \
        image = splitbuf_draw(buf, w, font); \
        image_write(image, imgout); \
        free(image); \
    } while (0)

void
animate(const struct command *p, size_t z, FILE *imgout)
{
    SplitBuffer buf[1];
    splitbuf_init(buf, z);
    printf("z: %d\n", (int) z);
    size_t w = 1000; // changing this to z will cause segmentation fault

    FILE *fontfile = fopen("font32.ppm", "rb");
    struct image *font = image_load(fontfile);
    fclose(fontfile);

    struct image *image;
    for (; p->op; p++) {
        switch (p->op) {
            case C_HALT: {
            } break;
            case C_WAIT: {
                int v = p->arg.v;
                while (v--)
                    FRAME();
            } break;
            case C_FORWARD: {
                int v = p->arg.v;
                while (v--) {
                    splitbuf_forward(buf);
                    FRAME();
                }
            } break;
            case C_BACKWARD: {
                int v = p->arg.v;
                while (v--) {
                    splitbuf_backward(buf);
                    FRAME();
                }
            } break;
            case C_QMOVE: {
                splitbuf_move(buf, p->arg.v);
            } break;
            case C_INSERT: {
                splitbuf_insert(buf, p->arg.v);
                FRAME();
            } break;
            case C_QINSERT: {
                splitbuf_insert(buf, p->arg.v);
            } break;
            case C_QSTRING: {
                splitbuf_inserts(buf, p->arg.s);
            } break;
            case C_STRING: {
                for (const char *s = p->arg.s; *s; s++) {
                    splitbuf_insert(buf, *s);
                    FRAME();
                }
            } break;
            case C_DELETE: {
                int v = p->arg.v;
                while (v--) {
                    splitbuf_delete(buf);
                    FRAME();
                }
            } break;
            case C_BACKSPACE: {
                int v = p->arg.v;
                while (v--) {
                    splitbuf_backspace(buf);
                    FRAME();
                }
            } break;
        }
    }

    splitbuf_destroy(buf);
    free(font);
}

size_t splitbuf_total_size(SplitBuffer* b) {
  return b->preGap->size + b->postGap->size;
}

size_t compute_max_total_size(const struct command *ptr) {
    const struct command* p = ptr;
    SplitBuffer buf[1];
    splitbuf_init(buf, 8);
    size_t max_total_size = 0;

    for (; p->op; p++) {
        switch (p->op) {
            case C_HALT: {
            } break;
            case C_WAIT: {
            } break;
            case C_FORWARD: {
                int v = p->arg.v;
                while (v--) {
                    splitbuf_forward(buf);
                    max_total_size = MAX(max_total_size, splitbuf_total_size(buf));
                }
            } break;
            case C_BACKWARD: {
                int v = p->arg.v;
                while (v--) {
                    splitbuf_backward(buf);
                    max_total_size = MAX(max_total_size, splitbuf_total_size(buf));
                }
            } break;
            case C_QMOVE: {
                splitbuf_move(buf, p->arg.v);
                max_total_size = MAX(max_total_size, splitbuf_total_size(buf));
            } break;
            case C_INSERT: {
                splitbuf_insert(buf, p->arg.v);
                max_total_size = MAX(max_total_size, splitbuf_total_size(buf));
            } break;
            case C_QINSERT: {
                splitbuf_insert(buf, p->arg.v);
                max_total_size = MAX(max_total_size, splitbuf_total_size(buf));
            } break;
            case C_QSTRING: {
                splitbuf_inserts(buf, p->arg.s);
                max_total_size = MAX(max_total_size, splitbuf_total_size(buf));
            } break;
            case C_STRING: {
                for (const char *s = p->arg.s; *s; s++) {
                    splitbuf_insert(buf, *s);
                    max_total_size = MAX(max_total_size, splitbuf_total_size(buf));
                }
            } break;
            case C_DELETE: {
                int v = p->arg.v;
                while (v--) {
                    splitbuf_delete(buf);
                    max_total_size = MAX(max_total_size, splitbuf_total_size(buf));
                }
            } break;
            case C_BACKSPACE: {
                int v = p->arg.v;
                while (v--) {
                    splitbuf_backspace(buf);
                    max_total_size = MAX(max_total_size, splitbuf_total_size(buf));
                }
            } break;
        }
    }

    splitbuf_destroy(buf);
    return max_total_size;
}

void animate_optimise_width(const struct command *p, size_t offset, FILE *imgout) {
  size_t max_total_size = compute_max_total_size(p);
  printf("max_total_size: %d\n", (int) max_total_size);
  animate(p, max_total_size + offset, imgout);
}


int
main(void)
{
    FILE *f;

    printf("hello world\n");
    #define FPS 10 
    static const struct command intro[] = {
        {C_WAIT,      .arg.v = FPS},
        {C_STRING,    .arg.s = "This is a buffer."},
        /* "This is a buffer." */
        {C_WAIT,      .arg.v = FPS},
        {C_BACKWARD,  .arg.v = 7},
        {C_STRING,    .arg.s = "gap "},
        /* "This is a gap buffer. */
        {C_WAIT,      .arg.v = FPS},
        {C_BACKWARD,  .arg.v = 5},
        {C_WAIT,      .arg.v = FPS / 2},
        {C_BACKSPACE, .arg.v = 9},
        /* " gap buffer. */
        {C_INSERT,    .arg.v = 'A'},
        /* "A gap buffer. */
        {C_FORWARD,   .arg.v = 11},
        {C_WAIT,      .arg.v = FPS / 2},
        {C_STRING,    .arg.s = " is for clustered edits"},
        /* "A gap buffer is good at clustered edits." */
        {C_WAIT,      .arg.v = FPS / 2},
        {C_FORWARD,   .arg.v = 1},
        {C_WAIT,      .arg.v = FPS},
        {C_BACKWARD,  .arg.v = 16},
        {C_BACKSPACE, .arg.v = 24},
        {C_DELETE,    .arg.v = 1},
        {C_INSERT,    .arg.v = 'C'}, /* "Clustered edits.*/
        {C_FORWARD ,  .arg.v = 14},
        {C_STRING,    .arg.s = " are most efficient!"},
        {C_DELETE,    .arg.v = 1},
        /* "Clustered edits are most efficient." */
        {C_WAIT,      .arg.v = FPS},
        {C_BACKSPACE, .arg.v = 35},
        /* "" */
        {C_WAIT,      .arg.v = FPS},
        {C_HALT},
    };
    f = fopen("intro.ppm", "wb");
    // animate(intro, 38, f);
    animate_optimise_width(intro, 2, f);
    fclose(f);
}
