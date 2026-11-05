#pragma once

#include "Enemy.h"
#include "Pickup.h"
#include "Player.h"
#include "Ranking.h"
#include "ResourceManager.h"
#include "Upgrades.h"
#include "raylib.h"
#include <stdbool.h>

#define MAX_ENEMIES 32
#define MAX_GEMS 32

typedef enum GameState {
	STATE_MENU,
	STATE_GAMEPLAY,
	STATE_GAMEOVER,
	STATE_LEVELUP,
	STATE_COUNT
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
	UpgradeType upgradeChoices[UPGRADE_CHOICES];

} Game;

Game* createGame(void);
void updateGame(Game* game);
void drawGame(Game* game);
void destroyGame(Game* game);
void changeGameState(Game* game, GameState newState);
void resetGame(Game* game);
