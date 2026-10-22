#pragma once

#include "Enemy.h"
#include "Player.h"
#include "ResourceManager.h"

#define MAX_ENEMIES 16

typedef enum GameState {
	STATE_MENU,
	STATE_GAMEPLAY,
	STATE_GAMEOVER,
	STATE_COUNT // total de estados, não é uma tela
} GameState;

typedef struct Game {
	GameState currentState;
	float delta;
	ResourceManager* resourceManager;

	Player player;
	Enemy enemies[MAX_ENEMIES];
	Camera2D camera;
	float spawnTimer;

	float gameTimer;
} Game;

Game* createGame(void);
void updateGame(Game* game);
void drawGame(Game* game);
void destroyGame(Game* game);
void changeGameState(Game* game, GameState newState);
