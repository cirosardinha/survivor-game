#include <stdlib.h>

#include "Collision.h"
#include "Combat.h"
#include "Game.h"
#include "Input.h"
#include "Map.h"
#include "config.h"
#include "raylib.h"
#include "raymath.h"
#include "ui.h"
#include <string.h>

#define FIRST_SPAWN_DELAY 0.5f
#define SPAWN_ENEMY_INTERVAL 5.0f
#define PICKUP_SPAWN_MIN 15
#define PICKUP_SPAWN_MAX 35

static void updateMenu(Game* game);
static void drawMenu(Game* game);

static void updateGameplay(Game* game);
static void drawGameplay(Game* game);

static void updateGameOver(Game* game);
static void drawGameOver(Game* game);

static void updateCamera(Game* game);

static void resetGame(Game* game);

static void spawnEnemy(Game* game);
static Texture2D getEnemyTexture(const ResourceManager* rm, EnemyType type);
static Vector2 randomSpawnPosition(const Game* game);

static Vector2 randomPickupPosition(const Game* game);
static float randomPickupDelay(void);
static void spawnGem(Game* game, Vector2 pos);

typedef struct StateFunctions {
	void (*update)(Game* game);
	void (*draw)(Game* game);
} StateFunctions;

static const StateFunctions stateTable[STATE_COUNT] = {
    [STATE_MENU] = {updateMenu, drawMenu},
    [STATE_GAMEPLAY] = {updateGameplay, drawGameplay},
    [STATE_GAMEOVER] = {updateGameOver, drawGameOver},
};

Game* createGame(void) {
	Game* game = calloc(1, sizeof(Game));
	if (game == NULL) exit(EXIT_FAILURE);
	game->currentState = STATE_MENU;
	game->delta = 0.0f;
	game->resourceManager = createResourceManager();
	loadResources(game->resourceManager);

	initMap();
	initPlayer(&game->player, game->resourceManager->playerTexture);
	game->spawnTimer = FIRST_SPAWN_DELAY;
	game->gameTimer = 0.0f;

	game->pickup.active = false;
	game->pickupSpawnTimer = randomPickupDelay();

	game->camera.target = (Vector2){game->player.pos.x, game->player.pos.y};
	game->camera.offset = (Vector2){WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f};
	game->camera.rotation = 0.0f;
	game->camera.zoom = 1.0f;

	return game;
}

void updateGame(Game* game) {
	game->delta = GetFrameTime();
	if (game->delta > MAX_DELTA) game->delta = MAX_DELTA;
	stateTable[game->currentState].update(game);
}

void drawGame(Game* game) {
	stateTable[game->currentState].draw(game);
}

void changeGameState(Game* game, GameState newState) {
	game->currentState = newState;
}

void destroyGame(Game* game) {
	destroyResourceManager(game->resourceManager);
	free(game);
}

static void drawCenteredText(const char* text) {
	DrawText(
	    text, (WINDOW_WIDTH - MeasureText(text, 20)) / 2,
	    WINDOW_HEIGHT / 2 - 10, 20, DARKGRAY);
}

static void updateMenu(Game* game) {
	if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		changeGameState(game, STATE_GAMEPLAY);
		resetGame(game);
	}
}

static void drawMenu(Game* game) {
	drawCenteredText("Toque ou ENTER para comecar");
}

static void updateGameplay(Game* game) {
	game->gameTimer += game->delta;
	if (game->player.health <= 0) {
		changeGameState(game, STATE_GAMEOVER);
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

	updatePlayer(&game->player, game->delta);
	updatePlayerAttack(&game->player, game->enemies, MAX_ENEMIES, game->delta);

	game->spawnTimer -= game->delta;
	if (game->spawnTimer <= 0.0f) {
		spawnEnemy(game);
		game->spawnTimer = SPAWN_ENEMY_INTERVAL;
	}

	for (int i = 0; i < MAX_ENEMIES; i++) {
		Enemy* enemy = &game->enemies[i];

		if (enemy->justDied) {
			enemy->justDied = false;
			Vector2 center = {
			    enemy->pos.x + enemy->stats.size / 2.0f,
			    enemy->pos.y + enemy->stats.size / 2.0f,
			};
			spawnGem(game, center);
		}

		if (!enemy->active) continue;

		updateEnemy(enemy, &game->player, game->delta);
		resolveEnemyPlayerCollision(enemy, &game->player, game->delta);
		updateEnemyAttack(enemy, &game->player, game->delta);
	}

	for (int i = 0; i < MAX_GEMS; i++) {
		updatePickup(&game->gems[i], &game->player, game->delta);
	}

	resolveAllEnemiesCollision(game->enemies, MAX_ENEMIES, game->delta);

	updateCamera(game);
}

static void drawGameplay(Game* game) {
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

	drawPlayerHealth(&game->player);
	drawTimer(game->gameTimer);
	drawScore(&game->player);
}

static void updateGameOver(Game* game) {
	Vector2 mouse = GetMousePosition();

	Rectangle menuButton = {
	    WINDOW_WIDTH / 2.0f - 120, WINDOW_HEIGHT / 2.0f - 20, 240, 50};

	Rectangle resetButton = {
	    WINDOW_WIDTH / 2.0f - 120, WINDOW_HEIGHT / 2.0f + 45, 240, 50};

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		if (CheckCollisionPointRec(mouse, menuButton)) {
			changeGameState(game, STATE_MENU);
		} else if (CheckCollisionPointRec(mouse, resetButton)) {
			resetGame(game);
			changeGameState(game, STATE_GAMEPLAY);
		}
	}
}

static void drawGameOver(Game* game) {
	DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, (Color){0, 0, 0, 180});

	const char* title = "GAME OVER";
	int titleSize = 50;

	DrawText(
	    title, (WINDOW_WIDTH - MeasureText(title, titleSize)) / 2,
	    WINDOW_HEIGHT / 2 - 100, titleSize, RED);

	Rectangle menuButton = {
	    WINDOW_WIDTH / 2.0f - 120, WINDOW_HEIGHT / 2.0f - 20, 240, 50};

	Rectangle resetButton = {
	    WINDOW_WIDTH / 2.0f - 120, WINDOW_HEIGHT / 2.0f + 45, 240, 50};

	Vector2 mouse = GetMousePosition();

	Color menuColor =
	    CheckCollisionPointRec(mouse, menuButton) ? DARKGRAY : GRAY;

	Color resetColor =
	    CheckCollisionPointRec(mouse, resetButton) ? DARKGRAY : GRAY;

	DrawRectangleRounded(menuButton, 0.2f, 8, menuColor);
	DrawRectangleRounded(resetButton, 0.2f, 8, resetColor);

	DrawText(
	    "Voltar ao menu",
	    (int) (menuButton.x +
		       (menuButton.width - MeasureText("Voltar ao menu", 20)) / 2),
	    (int) (menuButton.y + (menuButton.height - 20) / 2), 20, WHITE);

	DrawText(
	    "Reset",
	    (int) (resetButton.x +
		       (resetButton.width - MeasureText("Reset", 20)) / 2),
	    (int) (resetButton.y + (resetButton.height - 20) / 2), 20, WHITE);
}

static void updateCamera(Game* game) {
	float worldWidth = WORLD_WIDTH * TILE_SIZE;
	float worldHeight = WORLD_HEIGHT * TILE_SIZE;

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

static void spawnEnemy(Game* game) {
	for (int i = 0; i < MAX_ENEMIES; i++) {
		if (!game->enemies[i].active) {
			EnemyType type =
			    (EnemyType) GetRandomValue(0, ENEMY_TYPE_COUNT - 1);

			initEnemy(
			    &game->enemies[i], getEnemyTexture(game->resourceManager, type),
			    randomSpawnPosition(game), type);
			return;
		}
	}
}

static Texture2D getEnemyTexture(const ResourceManager* rm, EnemyType type) {
	switch (type) {
		case ENEMY_TYPE_SLIME:
			return rm->slimeTexture;
		case ENEMY_TYPE_BEETLE:
			return rm->beetleTexture;
		case ENEMY_TYPE_BAT:
			return rm->batTexture;
		default:
			return rm->slimeTexture;
	}
}

static Vector2 randomSpawnPosition(const Game* game) {
	float worldWidth = WORLD_WIDTH * TILE_SIZE;
	float worldHeight = WORLD_HEIGHT * TILE_SIZE;

	float radius = Vector2Length((Vector2){WINDOW_WIDTH, WINDOW_HEIGHT}) /
	                   (2.0f * game->camera.zoom) +
	               100.0f;

	Vector2 pos = game->player.pos;

	for (int tries = 0; tries < 15; tries++) {
		float angle = GetRandomValue(0, 360) * DEG2RAD;
		float r = radius + GetRandomValue(0, 100);
		pos.x = game->player.pos.x + (float) cos(angle) * r;
		pos.y = game->player.pos.y + (float) sin(angle) * r;

		if (pos.x >= 0 && pos.x <= worldWidth && pos.y >= 0 &&
		    pos.y <= worldHeight) {
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

	float angle = GetRandomValue(0, 359) * DEG2RAD;
	float dist = (float) GetRandomValue(150, 300);

	Vector2 pos = {
	    center.x + cosf(angle) * dist,
	    center.y + sinf(angle) * dist,
	};

	pos.x = Clamp(pos.x, TILE_SIZE, WORLD_WIDTH * TILE_SIZE - TILE_SIZE);
	pos.y = Clamp(pos.y, TILE_SIZE, WORLD_HEIGHT * TILE_SIZE - TILE_SIZE);
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

static void resetGame(Game* game) {
	memset(game->enemies, 0, sizeof(game->enemies));
	memset(game->gems, 0, sizeof(game->gems));
	initPlayer(&game->player, game->resourceManager->playerTexture);
	game->spawnTimer = FIRST_SPAWN_DELAY;
	game->pickup.active = false;
	game->pickupSpawnTimer = randomPickupDelay();
	game->gameTimer = 0.0f;
}
