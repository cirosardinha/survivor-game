#pragma once

#include "raylib.h"
#include <stdbool.h>

struct Player;

typedef enum EnemyType {
	ENEMY_TYPE_BAT,
	ENEMY_TYPE_BEETLE,
	ENEMY_TYPE_SLIME,
	ENEMY_TYPE_COUNT
} EnemyType;

typedef struct EnemyStats {
	int maxHealth;
	float speed;
	float size;
	float minDistance;

	float attackRange;
	int attackDamage;
	float attackCooldown;
} EnemyStats;

typedef struct Enemy {
	bool active;
	Vector2 pos;
	Vector2 vel;

	int health;
	float attackTimer;

	EnemyStats stats;
	EnemyType type;
	Texture2D texture;

	int frame;
	float frameTimer;
	float invincibleTimer;
} Enemy;

void initEnemy(Enemy* enemy, Texture2D texture, Vector2 pos, EnemyType type);
void updateEnemy(Enemy* enemy, struct Player* player, float delta);
void drawEnemy(Enemy* enemy);
