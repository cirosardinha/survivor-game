#pragma once

#include "Game.h"
#include "config.h"

typedef enum TileType {
	TILE_GRASS,
	TILE_GRASS_VAR,
	TILE_BUSH,
	TILE_ROCK,
	TILE_TYPE_COUNT
} TileType;

extern int map[WORLD_WIDTH][WORLD_HEIGHT];
void initMap();
bool isMapRectBlocked(Rectangle rect);
void drawMap(Game* game);
