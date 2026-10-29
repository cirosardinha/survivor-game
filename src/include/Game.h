#pragma once

#include "Enemy.h"
#include "Pickup.h"
#include "Player.h"
#include "Ranking.h"
#include "ResourceManager.h"

#define MAX_ENEMIES 32
#define MAX_GEMS 32

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
	float pickupSpawnTimer;

	float gameTimer;

	Pickup pickup;
	Pickup gems[MAX_GEMS];

	bool newRecord;

} Game;

Game* createGame(void);
void updateGame(Game* game);
void drawGame(Game* game);
void destroyGame(Game* game);
void changeGameState(Game* game, GameState newState);
