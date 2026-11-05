#pragma once

#include "Animation.h"
#include "ResourceManager.h"
#include "raylib.h"

typedef enum PlayerState {
	PLAYER_NORMAL,
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

typedef enum PlayerAnim {
	PLAYER_ANIM_IDLE,
	PLAYER_ANIM_WALK,
	PLAYER_ANIM_WALK_ATTACK,
	PLAYER_ANIM_ATTACK,
	PLAYER_ANIM_COUNT,
} PlayerAnim;

typedef struct Player {
	Vector2 pos;
	Vector2 vel;

	int health;
	int maxHealth;
	float speed;

	int score;
	int xp;
	int level;
	int attackDamage;
	float pickupRange;
	int pendingLevelUps;

	float invincibleTimer;
	float attackTimer;
	float attackCooldown;

	PlayerState state;
	PlayerDirection direction;

	Animation anims[PLAYER_ANIM_COUNT];
	AnimState animState;
	ResourceManager* resources;

} Player;

void initPlayer(Player* player, ResourceManager* rm);
void updatePlayer(Player* player, float delta);
void drawPlayer(Player* const player);
void startPlayerAttack(Player* player);
void damagePlayer(Player* player, int amount);
void healPlayer(Player* player, int amount);
void addPlayerXp(Player* player, int amount);
float getPlayerXpProgress(const Player* player);