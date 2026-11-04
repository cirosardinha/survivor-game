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
#define ROCK_VARIANTS 4
#define FLOWER_PERCENT 5
#define SPAWN_CLEAR_RADIUS 4

static void
placeCluster(int centerX, int centerY, int radius, int fillPercent, int type);
static bool isTileBlocked(int x, int y);
static bool isGrassAt(int x, int y);
static bool isSurroundedByGrass(int x, int y);
static void clearSpawnArea(void);
static void drawGroundTile(Game* game, int vertexX, int vertexY);

int map[WORLD_WIDTH][WORLD_HEIGHT];
static int variant[WORLD_WIDTH][WORLD_HEIGHT];

static const unsigned char groundTileRow[16] = {
    3, 3, 0, 0, 2, 0, 3, 1, 3, 1, 2, 0, 2, 2, 1, 1,
};
static const unsigned char groundTileCol[16] = {
    0, 1, 0, 3, 0, 1, 2, 1, 3, 0, 3, 2, 1, 2, 3, 2,
};

void initMap(void) {
	for (int x = 0; x < WORLD_WIDTH; x++) {
		for (int y = 0; y < WORLD_HEIGHT; y++) {
			variant[x][y] = 0;

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
		placeCluster(centerX, centerY, radius, CLUSTER_FILL_PERCENT, TILE_ROCK);
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
				}
			}
		}
	}
}

static bool isGrassAt(int x, int y) {
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (x >= WORLD_WIDTH) x = WORLD_WIDTH - 1;
	if (y >= WORLD_HEIGHT) y = WORLD_HEIGHT - 1;
	return map[x][y] != TILE_DIRT;
}

static bool isSurroundedByGrass(int x, int y) {
	for (int dx = -1; dx <= 1; dx++) {
		for (int dy = -1; dy <= 1; dy++) {
			if (!isGrassAt(x + dx, y + dy)) return false;
		}
	}
	return true;
}

static void drawGroundTile(Game* game, int vertexX, int vertexY) {
	int index = isGrassAt(vertexX - 1, vertexY - 1) * 8 +
	            isGrassAt(vertexX, vertexY - 1) * 4 +
	            isGrassAt(vertexX - 1, vertexY) * 2 +
	            isGrassAt(vertexX, vertexY);

	float half = TILE_SIZE / 2.0f;
	float halfMap = MAP_TILE_SIZE / 2.0f;

	Rectangle src = {
	    groundTileCol[index] * (float) TILE_SIZE,
	    groundTileRow[index] * (float) TILE_SIZE, (float) TILE_SIZE,
	    (float) TILE_SIZE};
	Rectangle dest = {
	    vertexX * MAP_TILE_SIZE - halfMap, vertexY * MAP_TILE_SIZE - halfMap,
	    MAP_TILE_SIZE, MAP_TILE_SIZE};

	if (vertexX == 0) {
		src.x += half;
		src.width -= half;
		dest.x += halfMap;
		dest.width -= halfMap;
	} else if (vertexX == WORLD_WIDTH) {
		src.width -= half;
		dest.width -= halfMap;
	}
	if (vertexY == 0) {
		src.y += half;
		src.height -= half;
		dest.y += halfMap;
		dest.height -= halfMap;
	} else if (vertexY == WORLD_HEIGHT) {
		src.height -= half;
		dest.height -= halfMap;
	}

	DrawTexturePro(
	    game->resourceManager->groundTilesetTexture, src, dest,
	    (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
}

static int clampInt(int value, int min, int max) {
	if (value < min) return min;
	if (value > max) return max;
	return value;
}

void drawMap(Game* game) {
	Vector2 topLeft = GetScreenToWorld2D((Vector2){0.0f, 0.0f}, game->camera);
	Vector2 bottomRight = GetScreenToWorld2D(
	    (Vector2){(float) GetScreenWidth(), (float) GetScreenHeight()},
	    game->camera);

	int minX =
	    clampInt((int) floorf(topLeft.x / MAP_TILE_SIZE) - 1, 0, WORLD_WIDTH);
	int minY =
	    clampInt((int) floorf(topLeft.y / MAP_TILE_SIZE) - 1, 0, WORLD_HEIGHT);
	int maxX = clampInt(
	    (int) floorf(bottomRight.x / MAP_TILE_SIZE) + 2, 0, WORLD_WIDTH);
	int maxY = clampInt(
	    (int) floorf(bottomRight.y / MAP_TILE_SIZE) + 2, 0, WORLD_HEIGHT);

	for (int x = minX; x <= maxX; x++) {
		for (int y = minY; y <= maxY; y++) {
			drawGroundTile(game, x, y);
		}
	}

	for (int x = minX; x < maxX && x < WORLD_WIDTH; x++) {
		for (int y = minY; y < maxY && y < WORLD_HEIGHT; y++) {
			int tile = map[x][y];

			Rectangle src = {0.0f, 0.0f, (float) TILE_SIZE, (float) TILE_SIZE};
			Rectangle dest = {
			    x * MAP_TILE_SIZE,
			    y * MAP_TILE_SIZE,
			    MAP_TILE_SIZE,
			    MAP_TILE_SIZE,
			};

			if (tile == TILE_GRASS_VAR && isSurroundedByGrass(x, y)) {
				DrawTexturePro(
				    game->resourceManager->grassVarTexture, src, dest,
				    (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
			} else if (tile == TILE_ROCK) {
				Rectangle rockSrc = {
				    (float) (variant[x][y] * TILE_SIZE),
				    0.0f,
				    (float) TILE_SIZE,
				    (float) TILE_SIZE,
				};
				DrawTexturePro(
				    game->resourceManager->rockTexture, rockSrc, dest,
				    (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
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
	return map[x][y] == TILE_ROCK;
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
