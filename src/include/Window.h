#pragma once

#include "Game.h"

typedef struct Window {
    Game *game;
} Window;

Window *createWindow(void);
void runWindow(Window *window); // InitWindow, loop, CloseWindow
void destroyWindow(Window *window);
