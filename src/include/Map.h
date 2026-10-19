#pragma once

#include "Game.h"
#include "config.h"

extern int map[WORLD_WIDTH][WORLD_HEIGHT];
void initMap();
void drawMap(Game* game);