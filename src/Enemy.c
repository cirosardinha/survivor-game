#include "Enemy.h"
#include "Player.h"
#include "config.h"
#include "raymath.h"

#define ENEMY_FRAME_COUNT 4
#define PLAYER_INVINCIBLE_TIME 1.0f
#define ENEMY_BLINK_INTERVAL 0.1f

static void followPlayer(Enemy* enemy, Player* player, float delta);
static void attackPlayer(Enemy* enemy, Player* player, float delta);
static void drawEnemyHealth(const Enemy* enemy);

static const EnemyStats enemyStats[ENEMY_TYPE_COUNT] = {
    [ENEMY_TYPE_BAT] =
        {
            .maxHealth = 30,
            .speed = 260.0f,
            .size = 64.0f,
            .minDistance = 40.0f,
            .attackRange = 48.0f,
            .attackDamage = 8,
            .attackCooldown = 0.8f,
        },

    [ENEMY_TYPE_BEETLE] =
        {
            .maxHealth = 120,
            .speed = 80.0f,
            .size = 64.0f,
            .minDistance = 48.0f,
            .attackRange = 56.0f,
            .attackDamage = 25,
            .attackCooldown = 1.5f,
        },

    [ENEMY_TYPE_SLIME] =
        {
            .maxHealth = 70,
            .speed = 130.0f,
            .size = 64.0f,
            .minDistance = 40.0f,
            .attackRange = 48.0f,
            .attackDamage = 12,
            .attackCooldown = 1.2f,
        },
};

void initEnemy(Enemy* enemy, Texture2D texture, Vector2 pos, EnemyType type) {
	const EnemyStats* stats = &enemyStats[type];

	enemy->active = true;
	enemy->pos = pos;
	enemy->vel = (Vector2){0.0f, 0.0f};

	enemy->health = stats->maxHealth;
	enemy->attackTimer = 0.0f;

	enemy->stats = *stats;
	enemy->type = type;

	enemy->texture = texture;
	enemy->frame = 0;
	enemy->frameTimer = 0.0f;
	enemy->invincibleTimer = 0.0f;
}

void updateEnemy(Enemy* enemy, struct Player* player, float delta) {
	if (!enemy->active) {
		return;
	}

	if (player->invincibleTimer > 0.0f) {
		player->invincibleTimer -= delta;
		if (player->invincibleTimer <= 0.0f) {
			player->invincibleTimer = 0.0f;
		}
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
	attackPlayer(enemy, player, delta);
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

	Color tint = WHITE;

	DrawTexturePro(
	    enemy->texture, src, dest, (Vector2){0.0f, 0.0f}, 0.0f, tint);

	drawEnemyHealth(enemy);
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

static void attackPlayer(Enemy* enemy, Player* player, float delta) {
	if (player->invincibleTimer > 0.0f) return;

	Vector2 enemyCenter = {
	    enemy->pos.x + enemy->stats.size / 2.0f,
	    enemy->pos.y + enemy->stats.size / 2.0f,
	};

	Vector2 playerCenter = {
	    player->pos.x + TILE_SIZE, player->pos.y + TILE_SIZE};

	float distance = Vector2Distance(enemyCenter, playerCenter);

	enemy->attackTimer += delta;

	if (distance <= enemy->stats.attackRange &&
	    enemy->attackTimer >= enemy->stats.attackCooldown) {
		player->health -= enemy->stats.attackDamage;
		player->invincibleTimer = PLAYER_INVINCIBLE_TIME;

		if (player->health <= 0) {
			player->health = 0;
		};

		enemy->attackTimer = 0.0f;
	}
}

static void drawEnemyHealth(const Enemy* enemy) {
	float barWidth = enemy->stats.size * 0.8f;
	float barHeight = 8.0f;

	float healthPercent = (float) enemy->health / enemy->stats.maxHealth;

	float x = enemy->pos.x + (enemy->stats.size - barWidth) / 2.0f;
	float y = enemy->pos.y - barHeight - 4.0f;

	DrawRectangle((int) x, (int) y, (int) barWidth, (int) barHeight, DARKGRAY);

	DrawRectangle(
	    (int) x, (int) y, (int) (barWidth * healthPercent), (int) barHeight,
	    RED);

	DrawRectangleLines(
	    (int) x, (int) y, (int) barWidth, (int) barHeight, WHITE);
}
