#include "Map.h"

#include "Game.h"
#include <math.h>

#define CLUSTER_COUNT 20
#define CLUSTER_MIN_RADIUS 1
#define CLUSTER_MAX_RADIUS 3
#define CLUSTER_FILL_PERCENT 75
#define DIRT_COUNT 9
#define DIRT_MIN_RADIUS 3
#define DIRT_MAX_RADIUS 5
#define DIRT_FILL_PERCENT 100
#define BUSH_VARIANTS 3
#define ROCK_VARIANTS 4
#define FLOWER_PERCENT 5
#define SPAWN_CLEAR_RADIUS 4

static void
placeCluster(int centerX, int centerY, int radius, int fillPercent, int type);
static bool isTileBlocked(int x, int y);
static void clearSpawnArea(void);

int map[WORLD_WIDTH][WORLD_HEIGHT];
static int variant[WORLD_WIDTH][WORLD_HEIGHT];

void initMap(void) {
	for (int x = 0; x < WORLD_WIDTH; x++) {
		for (int y = 0; y < WORLD_HEIGHT; y++) {
			if (GetRandomValue(0, 99) < FLOWER_PERCENT) {
				map[x][y] = TILE_GRASS_VAR;
			} else {
				map[x][y] = TILE_GRASS;
			}
		}
	}

	for (int i = 0; i < CLUSTER_COUNT; i++) {
		int centerX = GetRandomValue(0, WORLD_WIDTH - 1);
		int centerY = GetRandomValue(0, WORLD_HEIGHT - 1);
		int radius = GetRandomValue(CLUSTER_MIN_RADIUS, CLUSTER_MAX_RADIUS);
		int type = (GetRandomValue(0, 1) == 0) ? TILE_ROCK : TILE_BUSH;
		placeCluster(centerX, centerY, radius, CLUSTER_FILL_PERCENT, type);
	}

	for (int i = 0; i < DIRT_COUNT; i++) {
		int centerX = GetRandomValue(0, WORLD_WIDTH - 1);
		int centerY = GetRandomValue(0, WORLD_HEIGHT - 1);
		int radius = GetRandomValue(DIRT_MIN_RADIUS, DIRT_MAX_RADIUS);
		placeCluster(centerX, centerY, radius, DIRT_FILL_PERCENT, TILE_DIRT);
	}

	clearSpawnArea();
}

static void
placeCluster(int centerX, int centerY, int radius, int fillPercent, int type) {
	for (int x = centerX - radius; x <= centerX + radius; x++) {
		for (int y = centerY - radius; y <= centerY + radius; y++) {
			if (x < 0 || y < 0 || x >= WORLD_WIDTH || y >= WORLD_HEIGHT) {
				continue;
			}

			int dx = x - centerX;
			int dy = y - centerY;
			if (dx * dx + dy * dy > radius * radius) continue;

			if (type == TILE_DIRT && isTileBlocked(x, y)) continue;

			if (GetRandomValue(0, 99) < fillPercent) {
				map[x][y] = type;

				if (type == TILE_ROCK) {
					variant[x][y] = GetRandomValue(0, ROCK_VARIANTS - 1);
				} else if (type == TILE_BUSH) {
					variant[x][y] = GetRandomValue(0, BUSH_VARIANTS - 1);
				}
			}
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
			} else if (tile == TILE_DIRT) {
				ground = game->resourceManager->dirtTexture;
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

bool isMapRectBlocked(Rectangle rect) {
	int firstX = (int) floorf(rect.x / MAP_TILE_SIZE);
	int firstY = (int) floorf(rect.y / MAP_TILE_SIZE);
	int lastX = (int) floorf((rect.x + rect.width - 0.01f) / MAP_TILE_SIZE);
	int lastY = (int) floorf((rect.y + rect.height - 0.01f) / MAP_TILE_SIZE);

	for (int x = firstX; x <= lastX; x++) {
		for (int y = firstY; y <= lastY; y++) {
			if (isTileBlocked(x, y)) return true;
		}
	}

	return false;
}

static bool isTileBlocked(int x, int y) {
	if (x < 0 || y < 0 || x >= WORLD_WIDTH || y >= WORLD_HEIGHT) return true;
	return map[x][y] == TILE_ROCK || map[x][y] == TILE_BUSH;
}

static void clearSpawnArea(void) {
	float centerX = WORLD_WIDTH / 2.0f;
	float centerY = WORLD_HEIGHT / 2.0f;

	for (int x = 0; x < WORLD_WIDTH; x++) {
		for (int y = 0; y < WORLD_HEIGHT; y++) {
			float dx = x + 0.5f - centerX;
			float dy = y + 0.5f - centerY;

			if (dx * dx + dy * dy <= SPAWN_CLEAR_RADIUS * SPAWN_CLEAR_RADIUS) {
				map[x][y] = TILE_GRASS;
			}
		}
	}
}