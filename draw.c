#include "ASCIISymbols8x8Display.c"
#include "CNFG.h"
#include "utility.c"
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
typedef const u_int8_t Bitmap[8];
// clang-format off
static const Bitmap bitmap_letters[26] = {
    // A
    {
      0b00011000,
      0b00111100,
      0b01100110,
      0b01100110,
      0b01111110,
      0b01100110,
      0b01100110,
      0b01100110},

    // B
    {
      0b01111100,
      0b01100110,
      0b01100110,
      0b01111100,
      0b01100110,
      0b01100110,
      0b01100110,
      0b01111100},

    // C
    {
      0b00111100,
      0b01100110,
      0b01100000,
      0b01100000,
      0b01100000,
      0b01100000,
      0b01100110,
      0b00111100},

    // D
    {
	    0b01111000,
      0b01101100,
      0b01100110,
      0b01100110,
      0b01100110,
      0b01100110,
      0b01101100,
      0b01111000},

    // E
    {
	    0b01111110,
      0b01100000,
      0b01100000,
      0b01111100,
      0b01100000,
      0b01100000,
      0b01100000,
      0b01111110},

    // F
    {
	    0b01111110,
      0b01100000,
      0b01100000,
      0b01111100,
      0b01100000,
      0b01100000,
      0b01100000,
      0b01100000},

    // G
    {
	    0b00111100,
      0b01100110,
      0b01100000,
      0b01101110,
      0b01100110,
      0b01100110,
      0b01100110,
      0b00111100},

    // H
    {
	    0b01100110,
      0b01100110,
      0b01100110,
      0b01111110,
      0b01100110,
      0b01100110,
      0b01100110,
      0b01100110},

    // I
    {
	    0b00111100,
      0b00011000,
      0b00011000,
      0b00011000,
      0b00011000,
      0b00011000,
      0b00011000,
      0b00111100},

    // J
    {
	    0b00011110,
      0b00001100,
      0b00001100,
      0b00001100,
      0b00001100,
      0b01101100,
      0b01101100,
      0b00111000},

    // K
    {
	    0b01100110,
      0b01101100,
      0b01111000,
      0b01110000,
      0b01111000,
      0b01101100,
      0b01100110,
      0b01100110},

    // L
    { 
      0b01100000,
      0b01100000,
      0b01100000,
      0b01100000,
      0b01100000,
      0b01100000,
      0b01100000,
      0b01111110},

    // M
    {
      0b01100011,
      0b01110111,
      0b01111111,
      0b01101011,
      0b01100011,
      0b01100011,
      0b01100011,
      0b01100011},

    // N
    {
      0b01100110,
      0b01110110,
      0b01111110,
      0b01111110,
      0b01101110,
      0b01100110,
      0b01100110,
      0b01100110},

    // O
    {
      0b00111100,
      0b01100110,
      0b01100110,
      0b01100110,
      0b01100110,
      0b01100110,
      0b01100110,
      0b00111100},

    // P
    {
      0b01111100,
      0b01100110,
      0b01100110,
      0b01100110,
      0b01111100,
      0b01100000,
      0b01100000,
      0b01100000},

    // Q
    {
      0b00111100,
      0b01100110,
      0b01100110,
      0b01100110,
      0b01101110,
      0b01100110,
      0b00111100,
      0b00000110},

    // R
    {
      0b01111100,
      0b01100110,
      0b01100110,
      0b01111100,
      0b01111000,
      0b01101100,
      0b01100110,
      0b01100110},

    // S
    {
      0b00111100,
      0b01100110,
      0b01100000,
      0b00111100,
      0b00000110,
      0b00000110,
      0b01100110,
      0b00111100},

    // T
    {
      0b01111110,
      0b01011010,
      0b00011000,
      0b00011000,
      0b00011000,
      0b00011000,
      0b00011000,
      0b00011000},

    // U
    {
      0b01100110,
      0b01100110,
      0b01100110,
      0b01100110,
      0b01100110,
      0b01100110,
      0b01100110,
      0b00111100},

    // V
    {
      0b01100110,
      0b01100110,
      0b01100110,
      0b01100110,
      0b01100110,
      0b00111100,
      0b00111100,
      0b00011000},

    // W
    {
      0b01100011,
      0b01100011,
      0b01100011,
      0b01101011,
      0b01111111,
      0b01110111,
      0b01100011,
      0b01100011},

    // X
    {
      0b01100110,
      0b01100110,
      0b00111100,
      0b00011000,
      0b00111100,
      0b01100110,
      0b01100110,
      0b01100110},

    // Y
    {
      0b01100110,
      0b01100110,
      0b00111100,
      0b00011000,
      0b00011000,
      0b00011000,
      0b00011000,
      0b00011000},

    // Z
    {
      0b01111110,
      0b00000110,
      0b00001100,
      0b00011000,
      0b00110000,
      0b01100000,
      0b01100000,
      0b01111110},
};
// clang-format on

Bitmap bitmap_numbers[10] = {
    {
        0b00111100,
        0b01100110,
        0b11000011,
        0b11000011,
        0b11000011,
        0b11000011,
        0b01100110,
        0b00111100,
    },

    /* 1 */
    {
        0b00011000,
        0b00111000,
        0b00011000,
        0b00011000,
        0b00011000,
        0b00011000,
        0b00011000,
        0b01111110,
    },

    /* 2 */
    {
        0b00111100,
        0b01100110,
        0b00000110,
        0b00001100,
        0b00011000,
        0b00110000,
        0b01100000,
        0b01111110,
    },

    /* 3 */
    {
        0b00111100,
        0b01100110,
        0b00000110,
        0b00011100,
        0b00000110,
        0b00000110,
        0b01100110,
        0b00111100,
    },

    /* 4 */
    {
        0b00001100,
        0b00011100,
        0b00111100,
        0b01101100,
        0b11001100,
        0b01111110,
        0b00001100,
        0b00001100,
    },

    /* 5 */
    {
        0b01111110,
        0b01100000,
        0b01100000,
        0b01111100,
        0b00000110,
        0b00000110,
        0b01100110,
        0b00111100,
    },

    /* 6 */
    {
        0b00111100,
        0b01100110,
        0b01100000,
        0b01111100,
        0b01100110,
        0b01100110,
        0b01100110,
        0b00111100,
    },

    /* 7 */
    {
        0b01111110,
        0b00000110,
        0b00001100,
        0b00001100,
        0b00011000,
        0b00011000,
        0b00110000,
        0b00110000,
    },

    /* 8 */
    {
        0b00111100,
        0b01100110,
        0b01100110,
        0b00111100,
        0b01100110,
        0b01100110,
        0b01100110,
        0b00111100,
    },

    /* 9 */
    {
        0b00111100,
        0b01100110,
        0b01100110,
        0b01100110,
        0b00111110,
        0b00000110,
        0b01100110,
        0b00111100,
    },
};
static const uint32_t minesweeper_colors[9] = {
    0x00000000, // 0 - invisible
    0x0000ffff, // 1 - blue
    0x008000ff, // 2 - green
    0xff0000ff, // 3 - red
    0x000080ff, // 4 - dark blue
    0x800000ff, // 5 - dark red / maroon
    0x008080ff, // 6 - cyan / teal
    0x000000ff, // 7 - black
    0x808080ff, // 8 - gray
};
// clang-format off
static const Bitmap bitmap_redraw_arrow = {
    0b00000000,
    0b00111100, 
    0b01000010, 
    0b01000010,
    0b01001010, 
    0b01001100, 
    0b00101110, 
    0b00000000,
};
static const Bitmap bitmap_bomb = {
    0b00110000,
    0b00011000,
    0b00111100,
    0b01111110,
    0b01111110,
    0b01111110,
    0b01111110,
    0b00111100,
};

static const Bitmap bitmap_bomb_highlight = {
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000100,
    0b00000000,
    0b00000000,
    0b00000000,
};
static const Bitmap _bitmap_explosion = {
    0b00010000,
    0b01001100,
    0b01010010,
    0b00101110,
    0b11100100,
    0b01001010,
    0b00101100,
    0b00110100,
};

static const Bitmap _bitmap_explosion_highlight = {
    0b00000000,
    0b00000000,
    0b00100100,
    0b00010000,
    0b00011010,
    0b00110101,
    0b01010010,
    0b00000000,
};
static const Bitmap bitmap_explosion = {
    0b01110110,
    0b11001011,
    0b11101101,
    0b00000000,
    0b01111110,
    0b00000000,
    0b00001000,
    0b00100100,
};

static const Bitmap bitmap_explosion_highlight = {
    0b00001000,
    0b00110100,
    0b00010110,
    0b00011000,
    0b00000000,
    0b00011001,
    0b00010000,
    0b00011000,
};
// clang-format on

void ui_draw_bitmap(int px, int py, int size, double scaling, Bitmap b,
                    uint32_t color) {
  if ((color & 0x000000ff) == 0)
    return;
  int pixel_size = (double)size * scaling / 8.0;
  int final_size = pixel_size * 8;
  int border_size = size - final_size;
  CNFGLastColor = color;
  for (int y = 0; y < 8; y++) {
    u_int8_t row = b[y];
    for (int x = 0; x < 8; x++) {
      if (row & (1 << (7 - x))) {
        int ox = px + x * pixel_size + border_size / 2;
        int oy = py + y * pixel_size + border_size / 2;
        CNFGTackRectangle(ox, oy, ox + pixel_size, oy + pixel_size);
      }
    }
  }
}
void ui_draw_number(int x, int y, int size, u_int8_t number, uint32_t fg,
                    uint32_t bg) {
  CNFGLastColor = bg;
  CNFGTackRectangle(x, y, x + size, y + size);
  // char *numbers[] = {
  //     "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
  // };
  CNFGColor(0x000000ff);
  ui_draw_bitmap(x, y, size, 0.8, bitmap_numbers[number], fg);
  // CNFGPenX = x + size / 5;
  // CNFGPenY = y + size / 5;
  // CNFGDrawText(numbers[number], size / 5);
};
void ui_draw_bomb(int x, int y, int size, uint32_t fg, uint32_t hl,
                  uint32_t bg) {
  double scaling = 0.8;
  CNFGLastColor = bg;
  CNFGTackRectangle(x, y, x + size, y + size);
  ui_draw_bitmap(x, y, size, scaling, bitmap_bomb, fg);
  ui_draw_bitmap(x, y, size, scaling, bitmap_bomb_highlight, hl);
}
void ui_draw_explosion(int x, int y, int size, uint32_t fg, uint32_t hl,
                       uint32_t bg) {
  double scaling = 0.8;
  CNFGLastColor = bg;
  CNFGTackRectangle(x, y, x + size, y + size);
  ui_draw_bitmap(x, y, size, scaling, bitmap_explosion, fg);
  ui_draw_bitmap(x, y, size, scaling, bitmap_explosion_highlight, hl);
}

#define ALIGN_LEFT 0
#define ALIGN_CENTER 1
#define ALIGN_LRIGHT 2
void ui_draw_text(int x, int y, int height, int width, double scale, char *text,
                  uint32_t color, uint32_t bg, int align) {
  int text_length = strlen(text);
  int max_box_width = width / text_length;
  int box_size = min_int(scale * (double)height, max_box_width);
  int acual_width = text_length * box_size;
  int left_border = align * (width - acual_width) / 2;
  int top_border = (height - box_size) / 2;
  if (bg & 0x00000ff) {
    CNFGLastColor = bg;
    CNFGTackRectangle(x, y, x + width, y + height);
  }

  for (int idx = 0; idx < text_length; idx++) {
    int xo = x + left_border + idx * box_size;
    int yo = y + top_border;
    char c = text[idx];
    if (c >= 'a' && c <= 'z')
      c -= 'a' - 'A';

    if (c >= 'A' && c <= 'Z')
      ui_draw_bitmap(xo, yo, box_size, 1, bitmap_letters[c - 'A'], color);
    else {
      ui_draw_bitmap(xo, yo, box_size, 1, asciiSymbols[c - ' '], color);
    }
  }
}
