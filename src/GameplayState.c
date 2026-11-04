#include <math.h>

#include "Collision.h"
#include "Combat.h"
#include "Game.h"
#include "Input.h"
#include "Map.h"
#include "States.h"
#include "config.h"
#include "raylib.h"
#include "raymath.h"
#include "ui.h"

static void updateCamera(Game* game);

static void spawnEnemy(Game* game);
static float currentSpawnInterval(const Game* game);
static int maxUnlockedVariant(const Game* game);
static Vector2 randomSpawnPosition(const Game* game);

static Vector2 randomPickupPosition(const Game* game);
static float randomPickupDelay(void);
static void spawnGem(Game* game, Vector2 pos);

void updateGameplay(Game* game) {
	game->gameTimer += game->delta;
	if (game->player.health <= 0) {
		enterGameOver(game);
		return;
	}
	handleInput(&game->player);

	if (!game->pickup.active) {
		game->pickupSpawnTimer -= game->delta;
		if (game->pickupSpawnTimer <= 0.0f) {
			initPickup(
			    &game->pickup, game->resourceManager->healthPickupTexture,
			    PICKUP_HEALTH, randomPickupPosition(game));
			game->pickupSpawnTimer = randomPickupDelay();
		}
	}
	updatePickup(&game->pickup, &game->player, game->delta);

	Vector2 previousPlayerPos = game->player.pos;
	updatePlayer(&game->player, game->delta);

	resolvePlayerObstacleCollision(&game->player, previousPlayerPos);

	updatePlayerAttack(&game->player, game->enemies, MAX_ENEMIES, game->delta);

	game->spawnTimer -= game->delta;
	if (game->spawnTimer <= 0.0f) {
		spawnEnemy(game);
		game->spawnTimer = currentSpawnInterval(game);
	}

	for (int i = 0; i < MAX_ENEMIES; i++) {
		Enemy* enemy = &game->enemies[i];

		if (enemy->justDied) {
			enemy->justDied = false;
			Vector2 center = {
			    enemy->pos.x + enemy->stats.size / 2.0f,
			    enemy->pos.y + enemy->stats.size / 2.0f,
			};
			game->player.score += enemy->stats.scoreValue;
			spawnGem(game, center);
		}

		if (!enemy->active) continue;

		Vector2 previousEnemyPos = enemy->pos;
		updateEnemy(enemy, &game->player, game->delta);
		resolveEnemyObstacleCollision(enemy, previousEnemyPos);
		resolveEnemyPlayerCollision(enemy, &game->player, game->delta);
		updateEnemyAttack(enemy, &game->player, game->delta);
	}

	for (int i = 0; i < MAX_GEMS; i++) {
		updatePickup(&game->gems[i], &game->player, game->delta);
	}

	resolveAllEnemiesCollision(game->enemies, MAX_ENEMIES, game->delta);

	updateCamera(game);
}

void drawGameplay(Game* game) {
	BeginMode2D(game->camera);
	drawMap(game);
	drawPlayer(&game->player);

	for (int i = 0; i < MAX_ENEMIES; i++) {
		if (game->enemies[i].active) drawEnemy(&game->enemies[i]);
	}

	for (int i = 0; i < MAX_GEMS; i++) {
		drawPickup(&game->gems[i]);
	}

	drawPickup(&game->pickup);

	EndMode2D();

	drawPlayerHealth(&game->player, game->resourceManager);
	drawTimer(game->gameTimer, game->resourceManager);
	drawScore(&game->player, game->resourceManager);
	drawPlayerLevel(&game->player, game->resourceManager);
}

static void updateCamera(Game* game) {
	float worldWidth = WORLD_WIDTH * MAP_TILE_SIZE;
	float worldHeight = WORLD_HEIGHT * MAP_TILE_SIZE;

	float halfScreenWidth = (float) WINDOW_WIDTH / (2.0f * game->camera.zoom);
	float halfScreenHeight = (float) WINDOW_HEIGHT / (2.0f * game->camera.zoom);

	game->camera.target.x = game->player.pos.x + TILE_SIZE;
	game->camera.target.y = game->player.pos.y + TILE_SIZE;

	if (halfScreenWidth * 2 >= worldWidth) {
		game->camera.target.x = worldWidth / 2.0f;
	} else {
		if (game->camera.target.x < halfScreenWidth) {
			game->camera.target.x = halfScreenWidth;
		}
		if (game->camera.target.x > worldWidth - halfScreenWidth) {
			game->camera.target.x = worldWidth - halfScreenWidth;
		}
	}

	if (halfScreenHeight * 2.0f >= worldHeight) {
		game->camera.target.y = worldHeight / 2.0f;
	} else {
		if (game->camera.target.y < halfScreenHeight) {
			game->camera.target.y = halfScreenHeight;
		}

		if (game->camera.target.y > worldHeight - halfScreenHeight) {
			game->camera.target.y = worldHeight - halfScreenHeight;
		}
	}
}

static float currentSpawnInterval(const Game* game) {
	float t = game->gameTimer / SPAWN_RAMP_TIME;
	if (t > 1.0f) t = 1.0f;

	return SPAWN_INTERVAL_START +
	       (SPAWN_INTERVAL_MIN - SPAWN_INTERVAL_START) * t;
}

static int maxUnlockedVariant(const Game* game) {
	int unlocked = (int) (game->gameTimer / VARIANT_UNLOCK_INTERVAL);
	if (unlocked > ENEMY_VARIANTS - 1) unlocked = ENEMY_VARIANTS - 1;

	return unlocked;
}

static void spawnEnemy(Game* game) {
	for (int i = 0; i < MAX_ENEMIES; i++) {
		if (!game->enemies[i].active) {
			EnemyType type =
			    (EnemyType) GetRandomValue(0, ENEMY_TYPE_COUNT - 1);

			int variant = GetRandomValue(0, maxUnlockedVariant(game));

			initEnemy(
			    &game->enemies[i], game->resourceManager,
			    randomSpawnPosition(game), type, variant);
			return;
		}
	}
}

static Vector2 randomSpawnPosition(const Game* game) {
	float worldWidth = WORLD_WIDTH * MAP_TILE_SIZE;
	float worldHeight = WORLD_HEIGHT * MAP_TILE_SIZE;

	float radius = Vector2Length((Vector2){WINDOW_WIDTH, WINDOW_HEIGHT}) /
	                   (2.0f * game->camera.zoom) +
	               100.0f;

	Vector2 pos = game->player.pos;

	for (int tries = 0; tries < 30; tries++) {
		float angle = GetRandomValue(0, 360) * DEG2RAD;
		float r = radius + GetRandomValue(0, 100);
		pos.x = game->player.pos.x + cosf(angle) * r;
		pos.y = game->player.pos.y + sinf(angle) * r;

		Rectangle area = {pos.x, pos.y, MAP_TILE_SIZE, MAP_TILE_SIZE};

		if (pos.x >= 0 && pos.x <= worldWidth && pos.y >= 0 &&
		    pos.y <= worldHeight && !isMapRectBlocked(area)) {
			return pos;
		}
	}

	pos.x = Clamp(pos.x, 0, worldWidth);
	pos.y = Clamp(pos.y, 0, worldHeight);
	return pos;
}

static Vector2 randomPickupPosition(const Game* game) {
	Vector2 center = {
	    game->player.pos.x + PLAYER_WIDTH / 2.0f,
	    game->player.pos.y + PLAYER_HEIGHT / 2.0f,
	};

	Vector2 pos = center;

	for (int tries = 0; tries < 10; tries++) {
		float angle = GetRandomValue(0, 359) * DEG2RAD;
		float dist = (float) GetRandomValue(150, 300);

		pos.x = center.x + cosf(angle) * dist;
		pos.y = center.y + sinf(angle) * dist;

		pos.x =
		    Clamp(pos.x, TILE_SIZE, WORLD_WIDTH * MAP_TILE_SIZE - TILE_SIZE);
		pos.y =
		    Clamp(pos.y, TILE_SIZE, WORLD_HEIGHT * MAP_TILE_SIZE - TILE_SIZE);

		Rectangle area = {pos.x - 16.0f, pos.y - 16.0f, 32.0f, 32.0f};
		if (!isMapRectBlocked(area)) break;
	}

	return pos;
}

static float randomPickupDelay(void) {
	return (float) GetRandomValue(PICKUP_SPAWN_MIN, PICKUP_SPAWN_MAX);
}

static void spawnGem(Game* game, Vector2 pos) {
	for (int i = 0; i < MAX_GEMS; i++) {
		if (!game->gems[i].active) {
			initPickup(
			    &game->gems[i], game->resourceManager->xpGemTexture, PICKUP_XP,
			    pos);
			return;
		}
	}
}