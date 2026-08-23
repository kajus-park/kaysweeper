#include <android_native_app_glue.h>
#include <stdio.h>

#include "CNFG.h"

volatile int suspended;

extern struct android_app *gapp;
extern struct android_app *gapp;

int HandleDestroy() { return 0; }

void HandleSuspend() { suspended = 1; }

void HandleResume() { suspended = 0; }

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

void HandleButton(int x, int y, int button, int bDown) {
  lastbid = button;
  lastbuttonx = x;
  lastbuttony = y;
}

void HandleMotion(int x, int y, int mask) {
  lastmask = mask;
  lastmotionx = x;
  lastmotiony = y;
}

void main() {}
