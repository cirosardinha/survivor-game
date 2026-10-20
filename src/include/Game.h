#pragma once

#include "Enemy.h"
#include "Player.h"
#include "ResourceManager.h"

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
	Enemy enemy;
	Camera2D camera;
} Game;

Game* createGame(void);
void updateGame(Game* game);
void drawGame(Game* game);
void destroyGame(Game* game);
void changeGameState(Game* game, GameState newState);
