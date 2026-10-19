#include <stdlib.h>

#include "Game.h"
#include "Input.h"
#include "Map.h"
#include "config.h"
#include "raylib.h"

static void updateMenu(Game* game);
static void drawMenu(Game* game);

static void updateGameplay(Game* game);
static void drawGameplay(Game* game);

static void updateGameOver(Game* game);
static void drawGameOver(Game* game);

static void updateCamera(Game* game);

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
	handleInput(&game->player);

	updatePlayer(&game->player, game->delta);
	updateCamera(game);
}

static void drawGameplay(Game* game) {
	BeginMode2D(game->camera);
	drawMap(game);
	drawPlayer(&game->player);
	EndMode2D();
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
