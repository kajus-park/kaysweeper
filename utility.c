#ifndef KAY_UTILS
#define KAY_UTILS

#include <stdbool.h>
static inline int max_int(int a, int b) { return a > b ? a : b; }

static inline int min_int(int a, int b) { return a < b ? a : b; }

static inline int abs_int(int i) { return i < 0 ? -i : i; }

static inline bool in_range(int x, int start, int width) {
  return start <= x && x < start + width;
}

static inline bool in_rect(int x, int y, int start_x, int start_y, int width,
                           int height) {
  return in_range(x, start_x, width) && in_range(y, start_y, height);
}

#endif
