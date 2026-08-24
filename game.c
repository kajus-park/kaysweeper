
#include "./utility.c"
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

typedef int Winstate;
#define PLAYING 0
#define WON 1
#define LOST 2

typedef int Menu;
#define NONE 0
#define SETTINGS 2

typedef u_int8_t Field;
typedef struct Game {
  Winstate winstate;
  Menu menu;
  bool started;
  bool *_started;
  bool confirm_reset;
  double time_spent;
  int x_fields, y_fields;
  int total_bombs;
  int capacity;
  Field *fields;
  int level;
  bool vibrate;
} Game;

#define BOMBS_MASK 0b00001111
#define FLAGGED 0b00010000
#define REVEALED 0b00100000
#define IS_BOMB 0b01000000
#define FLOOD_FROM_HERE 0b10000000

#define LONG_PRESS_TIME 0.2
#define LONG_PRESS_MOVE 20

u_int8_t *game_get_field(Game g, int x, int y) {
  if (x < 0 || x >= g.x_fields)
    return NULL;
  if (y < 0 || y >= g.y_fields)
    return NULL;
  return &g.fields[x + y * g.x_fields];
}
bool game_flag_field(Game g, int x, int y) {
  if (!g.started) {
    return false;
  }
  Field *field = game_get_field(g, x, y);
  if (field == NULL)
    return false;
  if (*field & REVEALED) {
    return false;
  }
  *field ^= FLAGGED;
  return true;
}
int game_index_x(Game g, int idx) {
  if (idx < 0 || idx >= g.x_fields * g.y_fields) {
    return -1;
  }
  return idx % g.x_fields;
}
int game_index_y(Game g, int idx) {
  if (idx < 0 || idx >= g.x_fields * g.y_fields) {
    return -1;
  }
  return idx / g.x_fields;
}

void game_generate_bombs(Game g, int start_x, int start_y) {
  int placed_bombs = 0;
  memset(g.fields, 0, g.x_fields * g.y_fields);
  while (placed_bombs < g.total_bombs) {
    int rx = rand() % g.x_fields;
    int ry = rand() % g.y_fields;
    if (abs_int(start_x - rx) <= 1 && abs_int(start_y - ry) <= 1) {
      continue;
    }
    Field *field = game_get_field(g, rx, ry);
    if (*field & IS_BOMB)
      continue;
    *field |= IS_BOMB;
    placed_bombs++;
    for (int dy = -1; dy <= 1; dy++) {
      for (int dx = -1; dx <= 1; dx++) {
        int x = dx + rx;
        int y = dy + ry;
        Field *other = game_get_field(g, x, y);
        if (other != NULL)
          (*other)++;
        // (*other)++; // this works because as long as flood fill is not set
        // the number is positive
      }
    }
  }
}

// returns true if the field is a bomb
bool game_open_field_is_bomb(Game g, int x, int y) {
  if (!g.started) {
    game_generate_bombs(g, x, y);
    *g._started = true;
  }
  Field *field = game_get_field(g, x, y);
  if (*field & REVEALED || *field & FLAGGED)
    return false;
  *field |= REVEALED;
  if (*field & IS_BOMB) {
    return true;
  }
  printf("ther are %d bombs nearby\n", *field & BOMBS_MASK);
  bool is_zero = ((*field & BOMBS_MASK) == 0);
  if (is_zero) {
    *field |= FLOOD_FROM_HERE;
  }
  bool dirty = is_zero;
  while (dirty) {
    dirty = false;
    for (int idx = 0; idx < g.x_fields * g.y_fields; idx++) {
      field = &g.fields[idx];
      if (!(*field & FLOOD_FROM_HERE))
        continue;
      *field |= REVEALED;

      int cx = game_index_x(g, idx);
      int cy = game_index_y(g, idx);
      for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
          int x = cx + dx;
          int y = cy + dy;
          Field *other = game_get_field(g, x, y);
          if (other == NULL)
            continue;
          if (*other & REVEALED || *other & FLOOD_FROM_HERE)
            continue;
          *other |= REVEALED;
          bool is_zero = (*other & BOMBS_MASK) == 0;
          if (is_zero)
            *other |= FLOOD_FROM_HERE;
          dirty |= is_zero;
        }
      }
    }
  }
  // reset flood marker
  for (int idx = 0; idx < g.x_fields * g.y_fields; idx++) {
    field = &g.fields[idx];
    *field &= ~FLOOD_FROM_HERE;
  }
  return false;
}

// 0 keeps current config retutrn success
bool game_reset(Game *g, int width, int height, int num_bombs) {
  if (width > 0)
    g->x_fields = width;
  if (height > 0)
    g->y_fields = height;
  if (num_bombs > 0) {
    g->total_bombs = num_bombs;
  }
  g->time_spent = 0;
  g->started = false;
  g->_started = &g->started;
  g->winstate = PLAYING;
  if (g->capacity < g->x_fields * g->y_fields) {
    if (g->fields == NULL) {
      g->fields = malloc(g->x_fields * g->y_fields);
    } else {
      g->fields = realloc(g->fields, g->x_fields * g->y_fields);
    }
    if (g->fields == NULL) {
      printf("Error allocating game %s", strerror(errno));
      return false;
    }
  }
  memset(g->fields, 0, g->x_fields * g->y_fields);
  return true;
}

int game_count_flags(Game g) {
  int count = 0;
  for (int idx = 0; idx < g.x_fields * g.y_fields; idx++) {
    Field field = g.fields[idx];
    if (field & FLAGGED)
      count++;
  }
  return count;
}
bool game_won(Game g) {
  int count = 0;
  for (int idx = 0; idx < g.x_fields * g.y_fields; idx++) {
    Field field = g.fields[idx];
    if (!(field & IS_BOMB) && !(field & REVEALED))
      return false;
  }
  return true;
}

void game_add_time_to_timer(Game *g, double s) {
  if (g->started) {
    g->time_spent += s;
  }
}

typedef struct Difficulty {
  int width;
  int height;
  int num_bombs;
} Difficulty;

Difficulty levels[4] = {
    {.width = 11, .height = 23, .num_bombs = 38},
    {.width = 11, .height = 23, .num_bombs = 30},
    {.width = 11, .height = 23, .num_bombs = 26},
    {.width = 9, .height = 19, .num_bombs = 19},
};

void game_next_level_set(Game *g) {
  g->level++;
  g->level %= 4;
  Difficulty d = levels[g->level];
  game_reset(g, d.width, d.height, d.num_bombs);
}
