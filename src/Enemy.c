#include "Enemy.h"
#include "raymath.h"

#define ENEMY_FRAME_COUNT 4

static void followPlayer(Enemy* enemy, Player* player, float delta);

static const EnemyStats enemyStats[ENEMY_TYPE_COUNT] = {
    [ENEMY_TYPE_BAT] =
        {
            .maxHealth = 30,
            .speed = 260.0f,
            .size = 64.0f,
            .minDistance = 72.0f,
            .attackRange = 48.0f,
            .attackCooldown = 0.8f,
        },

    [ENEMY_TYPE_BEETLE] =
        {
            .maxHealth = 120,
            .speed = 80.0f,
            .size = 64.0f,
            .minDistance = 68.0f,
            .attackRange = 56.0f,
            .attackCooldown = 1.5f,
        },

    [ENEMY_TYPE_SLIME] =
        {
            .maxHealth = 70,
            .speed = 130.0f,
            .size = 64.0f,
            .minDistance = 64.0f,
            .attackRange = 48.0f,
            .attackCooldown = 1.2f,
        },
};

void initEnemy(Enemy* enemy, Texture2D texture, Vector2 pos, EnemyType type) {
	const EnemyStats* stats = &enemyStats[type];

	enemy->active = true;
	enemy->pos = pos;
	enemy->vel = (Vector2){0.0f, 0.0f};

	enemy->health = stats->maxHealth;

	enemy->stats = *stats;
	enemy->type = type;

	enemy->texture = texture;
	enemy->frame = 0;
	enemy->frameTimer = 0.0f;
}

void updateEnemy(Enemy* enemy, Player* player, float delta) {
	if (!enemy->active) {
		return;
	}

	enemy->frameTimer += delta;
	if (enemy->frameTimer >= 0.15f) {
		enemy->frameTimer -= 0.15f;
		enemy->frame++;
		if (enemy->frame >= ENEMY_FRAME_COUNT) {
			enemy->frame = 0;
		}
	}

	followPlayer(enemy, player, delta);
}

void drawEnemy(Enemy* enemy) {
	float frameWidth = (float) enemy->texture.width / ENEMY_FRAME_COUNT;
	float frameHeight = (float) enemy->texture.height;

	Rectangle src = {
	    (float) enemy->frame * frameWidth,
	    0.0f,
	    frameWidth,
	    frameHeight,
	};

	Rectangle dest = {
	    (float) enemy->pos.x,
	    (float) enemy->pos.y,
	    enemy->stats.size,
	    enemy->stats.size,
	};

	DrawTexturePro(
	    enemy->texture, src, dest, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
}

static void followPlayer(Enemy* enemy, Player* player, float delta) {
	Vector2 direction = Vector2Subtract(player->pos, enemy->pos);

	float length = Vector2Length(direction);

	if (length > enemy->stats.minDistance) {
		direction.x /= length;
		direction.y /= length;

		enemy->vel.x = direction.x * enemy->stats.speed;
		enemy->vel.y = direction.y * enemy->stats.speed;
	} else {
		enemy->vel = (Vector2){0.0f, 0.0f};
	}

	enemy->pos.x += enemy->vel.x * delta;
	enemy->pos.y += enemy->vel.y * delta;
}