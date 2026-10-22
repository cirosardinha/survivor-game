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

#define FIRST_SPAWN_DELAY 0.5f
#define SPAWN_ENEMY_INTERVAL 5.0f

static void updateMenu(Game* game);
static void drawMenu(Game* game);

static void updateGameplay(Game* game);
static void drawGameplay(Game* game);

static void updateGameOver(Game* game);
static void drawGameOver(Game* game);

static void updateCamera(Game* game);

static void spawnEnemy(Game* game);
static Texture2D getEnemyTexture(const ResourceManager* rm, EnemyType type);
static Vector2 randomSpawnPosition(const Game* game);

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
	Game* game = malloc(sizeof(Game));
	if (game == NULL) exit(EXIT_FAILURE);
	game->currentState = STATE_MENU;
	game->delta = 0.0f;
	game->resourceManager = createResourceManager();
	loadResources(game->resourceManager);

	initMap();
	initPlayer(&game->player, game->resourceManager->playerTexture);
	game->spawnTimer = FIRST_SPAWN_DELAY;

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
	}
}

static void drawMenu(Game* game) {
	drawCenteredText("Toque ou ENTER para comecar");
}

static void updateGameplay(Game* game) {
	if (game->player.health <= 0) {
		changeGameState(game, STATE_GAMEOVER);
	}
	handleInput(&game->player);

	updatePlayer(&game->player, game->delta);
	updatePlayerAttack(&game->player, game->enemies, MAX_ENEMIES, game->delta);

	game->spawnTimer -= game->delta;
	if (game->spawnTimer <= 0.0f) {
		spawnEnemy(game);
		game->spawnTimer = SPAWN_ENEMY_INTERVAL;
	}

	for (int i = 0; i < MAX_ENEMIES; i++) {
		Enemy* enemy = &game->enemies[i];

		if (!enemy->active) continue;

		updateEnemy(enemy, &game->player, game->delta);
		resolveEnemyPlayerCollision(enemy, &game->player, game->delta);
		updateEnemyAttack(enemy, &game->player, game->delta);
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

	EndMode2D();

	drawPlayerHealth(&game->player);
}

static void updateGameOver(Game* game) {
	if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		changeGameState(game, STATE_MENU);
	}
}

static void drawGameOver(Game* game) {
	drawCenteredText("Game Over - toque ou ENTER para voltar ao menu");
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
