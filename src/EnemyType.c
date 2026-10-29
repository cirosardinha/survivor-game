#include "EnemyType.h"

static const EnemyStats enemyStats[ENEMY_TYPE_COUNT] = {
    [ENEMY_TYPE_SLIME] =
        {
            .name = "slime",
            .maxHealth = 40,
            .speed = 90.0f,
            .size = 64.0f,
            .minDistance = 40.0f,
            .attackRange = 48.0f,
            .attackDamage = 6,
            .attackCooldown = 1.2f,
            .walkFrameTime = 0.12f,
            .attackFrameTime = 0.07f,
            .scoreValue = 10,
        },

    [ENEMY_TYPE_ORC] =
        {
            .name = "orc",
            .maxHealth = 100,
            .speed = 70.0f,
            .size = 64.0f,
            .minDistance = 48.0f,
            .attackRange = 56.0f,
            .attackDamage = 15,
            .attackCooldown = 2.0f,
            .walkFrameTime = 0.15f,
            .attackFrameTime = 0.08f,
            .scoreValue = 25,
        },

    [ENEMY_TYPE_PLANT] =
        {
            .name = "plant",
            .maxHealth = 70,
            .speed = 100.0f,
            .size = 64.0f,
            .minDistance = 40.0f,
            .attackRange = 48.0f,
            .attackDamage = 8,
            .attackCooldown = 1.4f,
            .walkFrameTime = 0.14f,
            .attackFrameTime = 0.08f,
            .scoreValue = 12,
        },

    [ENEMY_TYPE_VAMPIRE] =
        {
            .name = "vampire",
            .maxHealth = 30,
            .speed = 160.0f,
            .size = 64.0f,
            .minDistance = 40.0f,
            .attackRange = 48.0f,
            .attackDamage = 5,
            .attackCooldown = 1.0f,
            .walkFrameTime = 0.09f,
            .attackFrameTime = 0.06f,
            .scoreValue = 15,
        },
};

const EnemyStats* getEnemyStats(EnemyType type) {
	if (type < 0 || type >= ENEMY_TYPE_COUNT) {
		type = 0;
	}

	return &enemyStats[type];
}