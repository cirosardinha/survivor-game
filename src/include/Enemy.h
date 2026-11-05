#pragma once

#include "Animation.h"
#include "EnemyType.h"
#include "Player.h"
#include "ResourceManager.h"
#include "raylib.h"
#include <stdbool.h>

typedef enum EnemyAnim {
	ENEMY_ANIM_WALK,
	ENEMY_ANIM_ATTACK,
	ENEMY_ANIM_COUNT
} EnemyAnim;

typedef enum EnemyDirection {
	ENEMY_DOWN,
	ENEMY_UP,
	ENEMY_LEFT,
	ENEMY_RIGHT,
	ENEMY_DIRECTION_COUNT,
} EnemyDirection;

typedef struct Enemy {
	bool active;
	bool justDied;
	Vector2 pos;
	Vector2 vel;

	int health;
	float attackTimer;
	float invincibleTimer;
	float avoidTimer;
	int avoidSide;
	Vector2 stuckRef;
	float stuckTimer;

	EnemyStats stats;
	EnemyType type;
	int direction;

	Animation anims[ENEMY_ANIM_COUNT];
	AnimState animState;
	ResourceManager* resources;

} Enemy;

void initEnemy(
    Enemy* enemy, ResourceManager* rm, Vector2 pos, EnemyType type,
    int variant);
void updateEnemy(Enemy* enemy, Player* player, float delta);
void drawEnemy(Enemy* enemy);
void startEnemyAttack(Enemy* enemy);
void damageEnemy(Enemy* enemy, int amount);
