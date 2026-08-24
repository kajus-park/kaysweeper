// Copyright (c) 2011-2020 <>< Charles Lohr - Under the MIT/x11 or NewBSD
// License you choose.
//  NO WARRANTY! NO GUARANTEE OF SUPPORT! USE AT YOUR OWN RISK
//  Super basic test - see rawdrawandroid's thing for a more reasonable test.

#include "CNFGAndroid.h"
#include "os_generic.h"
#include <GLES3/gl3.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>
#include <android/sensor.h>
#include <android_native_app_glue.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define CNFG3D
#define CNFG_IMPLEMENTATION
#include "CNFG.h"
#include "utility.c"

#include "ASCIISymbols8x8Display.c"
#include "draw.c"
#include "game.c"

unsigned frames = 0;
unsigned long iframeno = 0;

#define GENLINEWIDTH 89
#define GENLINES 67
#define DEBUG true

int genlinelen = 0;
char genlog[(GENLINEWIDTH + 1) * (GENLINES + 1) + 2] = "log";
int genloglen;
int genloglines;
int firstnewline = -1;

void example_log_function(int readSize, char *buf) {
  static og_mutex_t *mt;
  if (!mt)
    mt = OGCreateMutex();
  OGLockMutex(mt);
  int i;
  for (i = 0; (readSize >= 0) ? (i <= readSize) : buf[i]; i++) {
    char c = buf[i];
    if (c == '\0')
      c = '\n';
    if ((c != '\n' && genlinelen >= GENLINEWIDTH) || c == '\n') {
      int k;
      genloglines++;
      if (genloglines >= GENLINES) {
        genloglen -= firstnewline + 1;
        int offset = firstnewline;
        firstnewline = -1;

        for (k = 0; k < genloglen; k++) {
          if ((genlog[k] = genlog[k + offset + 1]) == '\n' &&
              firstnewline < 0) {
            firstnewline = k;
          }
        }
        genlog[k] = 0;
        genloglines--;
      }
      genlinelen = 0;
      if (c != '\n') {
        genlog[genloglen + 1] = 0;
        genlog[genloglen++] = '\n';
      }
      if (firstnewline < 0)
        firstnewline = genloglen;
    }
    genlog[genloglen + 1] = 0;
    genlog[genloglen++] = c;
    if (c != '\n')
      genlinelen++;
  }

  OGUnlockMutex(mt);
}

volatile int suspended;

short screenx, screeny;
int lastbuttonx = 0;
int lastbuttony = 0;
int lastmotionx = 0;
int lastmotiony = 0;
int lastbid = 0;
int lastmask = 0;
int lastkey, lastkeydown;

static int keyboard_up;

void HandleKey(int keycode, int bDown) {
  lastkey = keycode;
  lastkeydown = bDown;
  if (keycode == 4) {
    AndroidSendToBack(1);
  }
}

static bool button_down = false;
static double button_down_start_time = {0};
static double last_handeled_press = {0};
static bool short_press = false;
typedef int Press_Type;
#define NONE 0
#define SHORT 1
#define LONG 2

void HandleButton(int x, int y, int button, int bDown) {
  // lastbid = button;
  if (button != 0)
    return;

  lastbuttonx = x;
  lastbuttony = y;
  if (bDown) {
    short_press = false;
    button_down = true;
    button_down_start_time = OGGetAbsoluteTime();
  } else if (button_down) {
    double held = OGGetAbsoluteTime() - button_down_start_time;
    if (held < LONG_PRESS_TIME) {
      short_press = true;
    }
    button_down = false;
  }
}

void HandleMotion(int x, int y, int mask) {
  lastmask = mask;
  lastmotionx = x;
  lastmotiony = y;
  if (button_down) {
    int dx = lastbuttonx - x;
    int dy = lastbuttony - y;
    if (dx * dx + dy * dy > LONG_PRESS_MOVE * LONG_PRESS_MOVE) {
      button_down = false;
      printf("moved too much");
    }
  }
}

Press_Type get_press_type() {
  if (last_handeled_press == button_down_start_time)
    return NONE;

  if (short_press) {
    short_press = false;
    last_handeled_press = button_down_start_time;
    return SHORT;
  }
  double held = OGGetAbsoluteTime() - button_down_start_time;
  if (held > LONG_PRESS_TIME) {
    last_handeled_press = button_down_start_time;
    return LONG;
  }

  return NONE;
}

extern struct android_app *gapp;

int HandleDestroy() { return 0; }

void HandleSuspend() { suspended = 1; }

void HandleResume() { suspended = 0; }

int main(int argc, char **argv) {
  int i, x, y;
  double ThisTime;
  double LastFPSTime = OGGetAbsoluteTime();
  double LastFrameTime = OGGetAbsoluteTime();
  double SecToWait;
  int linesegs = 0;

  CNFGBGColor = 0x400000ff;
  CNFGSetupFullscreen("Test Bench", 0);

  const char *assettext = "Not Found";
  AAsset *file = AAssetManager_open(gapp->activity->assetManager, "test.txt",
                                    AASSET_MODE_BUFFER);
  if (file) {
    size_t fileLength = AAsset_getLength(file);
    char *temp = malloc(fileLength + 1);
    memcpy(temp, AAsset_getBuffer(file), fileLength);
    temp[fileLength] = 0;
    assettext = temp;
  }

  Game g = {0};
  game_reset(&g, 5, 12, 5);
  // game_reset(&g, 11, 23, 60);

  while (1) {
    int i, pos;
    float f;
    iframeno++;
    RDPoint pto[3];

    CNFGHandleInput();

    if (suspended) {
      usleep(50000);
      continue;
    }

    CNFGClearFrame();
    CNFGColor(0xffffffff);
    CNFGGetDimensions(&screenx, &screeny);

    // // Mesh in background
    CNFGColor(0xffffffff);
    CNFGFlushRender();

    Press_Type _press = get_press_type();
    CNFGBGColor = 0x444444ff;
    int smallest_dim = min_int(screeny, screenx);
    int min_border = smallest_dim / 40;
    int top_bar = smallest_dim / 8;
    { // Top Bar
      int seconds = (int)g.time_spent % 60;
      int minutes = g.time_spent / 60;
      const int bufsize = 128;
      char buffer[bufsize];
      snprintf(buffer, bufsize, "%02d:%02d", minutes, seconds);
      ui_draw_text(top_bar / 5, top_bar / 5, top_bar * 3 / 5, screeny * 2 / 5,
                   1, &buffer[0], 0xffffffff, 0, 0);

      int buton_size = top_bar * 0.9;
      x = (screenx - buton_size) / 2;
      y = top_bar * 0.1;
      CNFGLastColor = g.confirm_reset ? 0xaa8844ff : 0xffffffff;
      CNFGTackRectangle(x, y, x + buton_size, y + buton_size);
      ui_draw_bitmap(x, y, buton_size, 1, bitmap_redraw_arrow, 0x00000ff);
      if (_press == SHORT) {
        if (in_rect(lastbuttonx, lastbuttony, x, y, buton_size, buton_size)) {
          if (g.confirm_reset) {
            game_reset(&g, 0, 0, 0);
            _press = NONE;
            g.confirm_reset = false;
            continue;
          } else {
            g.confirm_reset = true;
          }
          _press = NONE;
        } else {
          g.confirm_reset = false;
        }
      }

      int bombs_remaining = g.total_bombs - game_count_flags(g);
      snprintf(buffer, bufsize, "B%02d", bombs_remaining);
      int width = screeny * 2 / 5;
      ui_draw_text(screenx - top_bar / 5 - width, top_bar / 5, top_bar * 3 / 5,
                   width, 1, &buffer[0], 0xffffffff, 0, 2);
    }
    { // boxes
      int press = (g.winstate == PLAYING) ? _press : NONE;
      int gap = smallest_dim / 200;
      int max_x_box_size = (screenx + gap - 2 * min_border) / g.x_fields - gap;
      int max_y_box_size =
          (screeny + gap - 2 * min_border - top_bar) / g.y_fields - gap;
      int box_size = min_int(max_x_box_size, max_y_box_size);
      int offset = box_size + gap;
      int total_x_size = g.x_fields * offset - gap;
      int x_border = (screenx - total_x_size) / 2;
      for (int y = 0; y < g.y_fields; y++) {
        for (int x = 0; x < g.x_fields; x++) {
          int xo = x * offset + x_border;
          int yo = y * offset + min_border + top_bar;
          Field *field = game_get_field(g, x, y);

          if (press != NONE &&
              in_rect(lastbuttonx, lastbuttony, xo, yo, box_size, box_size)) {
            if (press == SHORT) {
              bool lost = game_open_field_is_bomb(g, x, y);
              if (lost) {
                g.winstate = LOST;
              }
              if (game_won(g)) {
                g.winstate = WON;
              }
              // printf("opening field %d %d", x, y);
            } else if (press == LONG) {
              game_flag_field(g, x, y);
            } else {
              printf("ERROR SHOULLD NOT HAPPEN press was %d\n", press);
            }
          }
          if (*field & REVEALED && !(*field & IS_BOMB)) {
            uint32_t fg = minesweeper_colors[*field & BOMBS_MASK];
            ui_draw_number(xo, yo, box_size, *field & BOMBS_MASK, fg,
                           0xffffffff);
          } else if (*field & FLAGGED) {
            CNFGLastColor = 0xffff00ff;
            CNFGTackRectangle(xo, yo, xo + box_size, yo + box_size);
          } else if ((DEBUG || g.winstate == LOST) && *field & IS_BOMB) {
            uint32_t bomb_background = 0x770000ff;
            if (*field & REVEALED) {
              ui_draw_explosion(xo, yo, box_size, 0xff44aaff, 0xffaa00ff,
                                bomb_background);
            } else {
              ui_draw_bomb(xo, yo, box_size, 0x181818ff, 0xffffffff,
                           bomb_background);
            }
          } else {
            CNFGLastColor = 0x888888ff;
            CNFGTackRectangle(xo, yo, xo + box_size, yo + box_size);
          }
        }
      }
    } // boxes
    { // Win/loss
      if (g.winstate == WON) {
        int x = 0;
        int y = screeny * 2 / 5;
        int height = screeny / 5;
        ui_draw_text(x, y, height, screenx, 1, "YOU WIN!", 0x88ff8888,
                     0x226622aa, 1);

      } else if (g.winstate == LOST) {
        int x = 0;
        int y = screeny * 2 / 5;
        int height = screeny / 5;
        ui_draw_text(x, y, height, screenx, 1, "YOU LOST!", 0xff888899,
                     0x662222bb, 1);
      }
    }
    // CNFGTackRectangle(600, 0, 950, 350);
    //
    // CNFGPenX = 10;
    // CNFGPenY = 10;
    //
    // // Text
    // pos = 0;
    // CNFGColor(0xffffffff);
    // for (i = 0; i < 1; i++) {
    //   int c;
    //   char tw[2] = {0, 0};
    //   for (c = 0; c < 256; c++) {
    //     tw[0] = c;
    //
    //     CNFGPenX = (c % 16) * 20 + 606;
    //     CNFGPenY = (c / 16) * 20 + 5;
    //     CNFGDrawText(tw, 4);
    //   }
    // }
    //
    // // Green triangles
    // CNFGPenX = 0;
    // CNFGPenY = 0;
    //
    // for (i = 0; i < 400; i++) {
    //   RDPoint pp[3];
    //   CNFGColor(0x00ff00ff);
    //   pp[0].x = (short)(50 * sin((float)(i + iframeno) * .01) + (i % 20)
    //   * 30); pp[0].y = (short)(50 * cos((float)(i + iframeno) * .01) + (i
    //   / 20)
    //   * 20); pp[1].x = (short)(20 * sin((float)(i + iframeno) * .01) + (i
    //   % 20) * 30); pp[1].y = (short)(50 * cos((float)(i + iframeno) *
    //   .01) + (i / 20) * 20); pp[2].x = (short)(10 * sin((float)(i +
    //   iframeno) * .01) + (i % 20) * 30); pp[2].y = (short)(30 *
    //   cos((float)(i + iframeno) * .01)
    //   + (i / 20) * 20); CNFGTackPoly(pp, 3);
    // }
    //
    CNFGPenX = 10;
    CNFGPenY = 600;
    CNFGLastColor = 0x4444ffff;
    CNFGDrawText(genlog, 4);

    frames++;
    CNFGSwapBuffers();

    ThisTime = OGGetAbsoluteTime();
    double delta_time = ThisTime - LastFrameTime;
    LastFrameTime = ThisTime;
    if (g.winstate == PLAYING) {
      game_add_time_to_timer(&g, delta_time);
    }

    if (ThisTime > LastFPSTime + 1) {
      printf("FPS: %d\n", frames);
      frames = 0;
      linesegs = 0;
      LastFPSTime += 1;
    }
  }

  return (0);
}
