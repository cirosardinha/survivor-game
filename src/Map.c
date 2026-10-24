#include "Map.h"

#include "Game.h"

#define CLUSTER_COUNT 8
#define CLUSTER_MIN_RADIUS 1
#define CLUSTER_MAX_RADIUS 3
#define CLUSTER_FILL_PERCENT 75
#define FLOWER_PERCENT 4

int map[WORLD_WIDTH][WORLD_HEIGHT];
static int variant[WORLD_WIDTH][WORLD_HEIGHT];

static void placeCluster(int centerX, int centerY, int type) {
	int radius = GetRandomValue(CLUSTER_MIN_RADIUS, CLUSTER_MAX_RADIUS);

	for (int x = centerX - radius; x <= centerX + radius; x++) {
		for (int y = centerY - radius; y <= centerY + radius; y++) {
			if (x < 0 || y < 0 || x >= WORLD_WIDTH || y >= WORLD_HEIGHT) {
				continue;
			}

			int dx = x - centerX;
			int dy = y - centerY;
			if (dx * dx + dy * dy > radius * radius) continue;

			if (GetRandomValue(0, 99) < CLUSTER_FILL_PERCENT) {
				map[x][y] = type;
			}
		}
	}
}

void initMap(void) {
	for (int x = 0; x < WORLD_WIDTH; x++) {
		for (int y = 0; y < WORLD_HEIGHT; y++) {
			if (GetRandomValue(0, 99) < FLOWER_PERCENT) {
				map[x][y] = TILE_GRASS_VAR;
			} else {
				map[x][y] = TILE_GRASS;
			}
			variant[x][y] = GetRandomValue(0, 2);
		}
	}

	for (int i = 0; i < CLUSTER_COUNT; i++) {
		int centerX = GetRandomValue(0, WORLD_WIDTH - 1);
		int centerY = GetRandomValue(0, WORLD_HEIGHT - 1);
		int type = (GetRandomValue(0, 1) == 0) ? TILE_ROCK : TILE_BUSH;
		placeCluster(centerX, centerY, type);
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