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
#define SPAWN_INTERVAL_START 5.0f
#define SPAWN_INTERVAL_MIN 1.0f
#define SPAWN_RAMP_TIME 240.0f
#define VARIANT_UNLOCK_INTERVAL 60.0f
#define PICKUP_SPAWN_MIN 15
#define PICKUP_SPAWN_MAX 35

static void updateMenu(Game* game);
static void drawMenu(Game* game);

static void updateGameplay(Game* game);
static void drawGameplay(Game* game);

static void updateGameOver(Game* game);
static void drawGameOver(Game* game);
static void enterGameOver(Game* game);
static void drawRankingList(int y);
static void drawCenteredLine(const char* text, int y, int size, Color color);
static Rectangle gameOverMenuButton(void);
static Rectangle gameOverResetButton(void);

static void updateCamera(Game* game);

static void resetGame(Game* game);

static void spawnEnemy(Game* game);
static float currentSpawnInterval(const Game* game);
static int maxUnlockedVariant(const Game* game);
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
	loadRanking();

	initMap();
	initPlayer(&game->player, game->resourceManager);
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
	drawPlayerLevel(&game->player);
}

static void enterGameOver(Game* game) {
	game->newRecord = addRankingScore(game->player.score);
	changeGameState(game, STATE_GAMEOVER);
}

static void updateGameOver(Game* game) {
	Vector2 mouse = GetMousePosition();

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		if (CheckCollisionPointRec(mouse, gameOverMenuButton())) {
			changeGameState(game, STATE_MENU);
		} else if (CheckCollisionPointRec(mouse, gameOverResetButton())) {
			resetGame(game);
			changeGameState(game, STATE_GAMEPLAY);
		}
	}
}

static void drawGameOver(Game* game) {
	DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, (Color){0, 0, 0, 180});

	drawCenteredLine("GAME OVER", 70, 50, RED);

	int minutes = (int) game->gameTimer / 60;
	int seconds = (int) game->gameTimer % 60;
	drawCenteredLine(
	    TextFormat(
	        "Pontos: %d    Tempo: %02d:%02d", game->player.score, minutes,
	        seconds),
	    140, 24, WHITE);

	if (game->newRecord) {
		drawCenteredLine("Novo recorde!", 175, 28, YELLOW);
	}

	drawRankingList(220);

	Rectangle menuButton = gameOverMenuButton();
	Rectangle resetButton = gameOverResetButton();

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

static void drawRankingList(int y) {
	drawCenteredLine("Melhores pontuacoes", y, 28, YELLOW);

	for (int i = 0; i < RANKING_SIZE; i++) {
		int score = getRankingScore(i);

		const char* text = score > 0 ? TextFormat("%d. %d", i + 1, score)
		                             : TextFormat("%d. -", i + 1);

		drawCenteredLine(text, y + 45 + i * 34, 24, WHITE);
	}
}

static void drawCenteredLine(const char* text, int y, int size, Color color) {
	DrawText(
	    text, (WINDOW_WIDTH - MeasureText(text, size)) / 2, y, size, color);
}

static Rectangle gameOverMenuButton(void) {
	return (Rectangle){WINDOW_WIDTH / 2.0f - 120, 430, 240, 50};
}

static Rectangle gameOverResetButton(void) {
	return (Rectangle){WINDOW_WIDTH / 2.0f - 120, 495, 240, 50};
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
		pos.x = game->player.pos.x + (float) cos(angle) * r;
		pos.y = game->player.pos.y + (float) sin(angle) * r;

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

static void resetGame(Game* game) {
	memset(game->enemies, 0, sizeof(game->enemies));
	memset(game->gems, 0, sizeof(game->gems));
	initPlayer(&game->player, game->resourceManager);
	game->spawnTimer = FIRST_SPAWN_DELAY;
	game->pickup.active = false;
	game->pickupSpawnTimer = randomPickupDelay();
	game->gameTimer = 0.0f;
}