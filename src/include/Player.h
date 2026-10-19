#pragma once

#include "raylib.h"

typedef enum PlayerState {
	PLAYER_WALKING,
	PLAYER_ATTACKING,
	PLAYER_STATE_COUNT,
} PlayerState;

typedef enum PlayerDirection {
	PLAYER_DOWN,
	PLAYER_LEFT,
	PLAYER_RIGHT,
	PLAYER_UP,
	PLAYER_DIRECTION_COUNT,
} PlayerDirection;

typedef struct Player {
	Vector2 pos;
	Vector2 vel;

	int health;
	int maxHealth;
	float speed;

	PlayerState state;
	PlayerDirection direction;
} Player;

void initPlayer(Player* player);
void updatePlayer(Player* player, float delta);
void drawPlayer(Player* const player);