#pragma once

#include "raylib.h"
#include <stdbool.h>

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
	float attackCooldown;
} EnemyStats;

typedef struct Enemy {
	bool active;
	Vector2 pos;
	Vector2 vel;

	int health;

	EnemyStats stats;
	EnemyType type;
	Texture2D texture;

	int frame;
	float frameTimer;
} Enemy;
void initEnemy(Enemy* enemy, Texture2D texture, Vector2 pos, EnemyType type);
void updateEnemy(Enemy* enemy, float delta);
void drawEnemy(Enemy* enemy);
