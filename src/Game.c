#include <stdlib.h>

#include "Game.h"
#include "config.h"
#include "raylib.h"

static void updateMenu(Game* game);
static void drawMenu(Game* game);

static void updateGameplay(Game* game);
static void drawGameplay(Game* game);

static void updateGameOver(Game* game);
static void drawGameOver(Game* game);

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
	if (IsKeyPressed(KEY_ESCAPE)) changeGameState(game, STATE_GAMEOVER);
}

static void drawGameplay(Game* game) {
	drawCenteredText("Jogando...");
}

static void updateGameOver(Game* game) {
	if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		changeGameState(game, STATE_MENU);
	}
}

static void drawGameOver(Game* game) {
	drawCenteredText("Game Over - toque ou ENTER para voltar ao menu");
}
