#include "Map.h"

#include "Game.h"
int map[WORLD_WIDTH][WORLD_HEIGHT];
void initMap(void) {
	for (int x = 0; x < WORLD_WIDTH; x++) {
		for (int y = 0; y < WORLD_HEIGHT; y++) {
			map[x][y] = 0;
		}
	}
}

void drawMap(Game* game) {
	for (int x = 0; x < WORLD_WIDTH; x++) {
		for (int y = 0; y < WORLD_HEIGHT; y++) {
			if (map[x][y] == 0) {
				DrawTexture(
				    game->resourceManager->grassTexture, x * TILE_SIZE,
				    y * TILE_SIZE, WHITE);
			}
		}
	}
}