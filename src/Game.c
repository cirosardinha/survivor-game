#include <stdlib.h>

#include "Game.h"
#include "Map.h"
#include "States.h"
#include "config.h"
#include <string.h>

typedef struct StateFunctions {
	void (*update)(Game* game);
	void (*draw)(Game* game);
} StateFunctions;

static const StateFunctions stateTable[STATE_COUNT] = {
    [STATE_MENU] = {updateMenu, drawMenu},
    [STATE_GAMEPLAY] = {updateGameplay, drawGameplay},
    [STATE_GAMEOVER] = {updateGameOver, drawGameOver},
    [STATE_LEVELUP] = {updateLevelUp, drawLevelUp},
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
	PlayMusicStream(game->resourceManager->backgroundMusic);

	game->spawnTimer = FIRST_SPAWN_DELAY;
	game->gameTimer = 0.0f;
	game->startTimer = 0.0f;

	game->pickup.active = false;
	game->pickupSpawnTimer =
	    (float) GetRandomValue(PICKUP_SPAWN_MIN, PICKUP_SPAWN_MAX);

	game->camera.target = (Vector2){game->player.pos.x, game->player.pos.y};
	game->camera.offset = (Vector2){WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f};
	game->camera.rotation = 0.0f;
	game->camera.zoom = 1.0f;

	return game;
}

void updateGame(Game* game) {
	UpdateMusicStream(game->resourceManager->backgroundMusic);
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

void resetGame(Game* game) {
	memset(game->enemies, 0, sizeof(game->enemies));
	memset(game->gems, 0, sizeof(game->gems));
	initPlayer(&game->player, game->resourceManager);
	game->spawnTimer = FIRST_SPAWN_DELAY;
	game->pickup.active = false;
	game->pickupSpawnTimer =
	    (float) GetRandomValue(PICKUP_SPAWN_MIN, PICKUP_SPAWN_MAX);
	game->gameTimer = 0.0f;
	game->startTimer = 0.0f;
}