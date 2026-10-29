#pragma once

#define ENEMY_VARIANTS 3

typedef enum EnemyType {
	ENEMY_TYPE_SLIME,
	ENEMY_TYPE_ORC,
	ENEMY_TYPE_PLANT,
	ENEMY_TYPE_VAMPIRE,
	ENEMY_TYPE_COUNT
} EnemyType;

typedef struct EnemyStats {
	const char* name;
	int maxHealth;
	float speed;
	float size;
	float minDistance;

	float attackRange;
	int attackDamage;
	float attackCooldown;

	int scoreValue;

	float walkFrameTime;
	float attackFrameTime;
} EnemyStats;

const EnemyStats* getEnemyStats(EnemyType type);
