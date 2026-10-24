#include "Map.h"

#include "Game.h"

int map[WORLD_WIDTH][WORLD_HEIGHT];
static int variant[WORLD_WIDTH][WORLD_HEIGHT];

void initMap(void) {
	for (int x = 0; x < WORLD_WIDTH; x++) {
		for (int y = 0; y < WORLD_HEIGHT; y++) {
			int roll = GetRandomValue(0, 99);

			if (roll < 4) {
				map[x][y] = TILE_ROCK;
			} else if (roll < 7) {
				map[x][y] = TILE_BUSH;
			} else if (roll < 11) {
				map[x][y] = TILE_GRASS_VAR;
			} else {
				map[x][y] = TILE_GRASS;
			}

			variant[x][y] = GetRandomValue(0, 2);
		}
	}
}

void drawMap(Game* game) {
	for (int x = 0; x < WORLD_WIDTH; x++) {
		for (int y = 0; y < WORLD_HEIGHT; y++) {
			int tile = map[x][y];

			Rectangle src = {0.0f, 0.0f, (float) TILE_SIZE, (float) TILE_SIZE};
			Rectangle dest = {
			    x * MAP_TILE_SIZE,
			    y * MAP_TILE_SIZE,
			    MAP_TILE_SIZE,
			    MAP_TILE_SIZE,
			};

			Texture2D ground = game->resourceManager->grassTexture;
			if (tile == TILE_GRASS_VAR) {
				ground = game->resourceManager->grassVarTexture;
			}

			DrawTexturePro(
			    ground, src, dest, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);

			if (tile == TILE_BUSH || tile == TILE_ROCK) {
				Texture2D obstacle = game->resourceManager->bushTexture;
				if (tile == TILE_ROCK) {
					obstacle = game->resourceManager->rockTexture;
				}

				Rectangle obstacleSrc = {
				    (float) (variant[x][y] * TILE_SIZE),
				    0.0f,
				    (float) TILE_SIZE,
				    (float) TILE_SIZE,
				};
				DrawTexturePro(
				    obstacle, obstacleSrc, dest, (Vector2){0.0f, 0.0f}, 0.0f,
				    WHITE);
			}
		}
	}
}