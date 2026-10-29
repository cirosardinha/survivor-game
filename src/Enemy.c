#include "Enemy.h"
#include "Player.h"
#include "config.h"
#include "raymath.h"

#define ENEMY_BLINK_INTERVAL 0.1f
#define ENEMY_INVINCIBLE_TIME 0.3f
#define VARIANT_HEALTH_BONUS 0.5f
#define VARIANT_DAMAGE_BONUS 0.25f

static void followPlayer(Enemy* enemy, Player* player, float delta);
static void drawEnemyHealth(const Enemy* enemy);

void initEnemy(
    Enemy* enemy, ResourceManager* rm, Vector2 pos, EnemyType type,
    int variant) {
	const EnemyStats* stats = getEnemyStats(type);

	enemy->active = true;
	enemy->justDied = false;
	enemy->pos = pos;
	enemy->vel = (Vector2){0.0f, 0.0f};

	enemy->attackTimer = 0.0f;
	enemy->invincibleTimer = 0.0f;

	enemy->stats = *stats;
	enemy->stats.maxHealth =
	    (int) (stats->maxHealth * (1.0f + variant * VARIANT_HEALTH_BONUS));
	enemy->stats.attackDamage =
	    (int) (stats->attackDamage * (1.0f + variant * VARIANT_DAMAGE_BONUS));
	enemy->stats.scoreValue = stats->scoreValue * (1 + variant);
	enemy->type = type;
	enemy->direction = ENEMY_DOWN;
	enemy->health = enemy->stats.maxHealth;

	initAnimation(
	    &enemy->anims[ENEMY_ANIM_WALK], rm->enemyWalkTextures[type][variant],
	    stats->walkFrameTime, true);
	initAnimation(
	    &enemy->anims[ENEMY_ANIM_ATTACK],
	    rm->enemyAttackTextures[type][variant], stats->attackFrameTime, false);
	enemy->animState = (AnimState){.current = ENEMY_ANIM_WALK};
}

void updateEnemy(Enemy* enemy, struct Player* player, float delta) {
	if (enemy->invincibleTimer > 0.0f) {
		enemy->invincibleTimer -= delta;
		if (enemy->invincibleTimer <= 0.0f) {
			enemy->invincibleTimer = 0.0f;
		}
	}
	if (!enemy->active) {
		return;
	}

	if (enemy->animState.current == ENEMY_ANIM_ATTACK) {
		enemy->vel = (Vector2){0.0f, 0.0f};

		if (enemy->animState.finished) {
			setAnimation(&enemy->animState, ENEMY_ANIM_WALK);
		}
	} else {
		followPlayer(enemy, player, delta);
	}

	updateAnimation(
	    &enemy->anims[enemy->animState.current], &enemy->animState, delta);
}

void startEnemyAttack(Enemy* enemy) {
	setAnimation(&enemy->animState, ENEMY_ANIM_ATTACK);
}

void drawEnemy(Enemy* enemy) {
	if (!enemy->active) return;

	Color tint = WHITE;

	if (enemy->invincibleTimer > 0.0f) {
		int phase = (int) (enemy->invincibleTimer / ENEMY_BLINK_INTERVAL);
		if (phase % 2 == 0) {
			tint = Fade(RED, 0.3f);
		}
	}

	Vector2 spritePos = {
	    enemy->pos.x + enemy->stats.size / 2.0f - SPRITE_SIZE / 2.0f,
	    enemy->pos.y + enemy->stats.size / 2.0f - SPRITE_SIZE / 2.0f,
	};

	drawAnimation(
	    &enemy->anims[enemy->animState.current], &enemy->animState, spritePos,
	    enemy->direction, tint);

	drawEnemyHealth(enemy);
}

static void followPlayer(Enemy* enemy, Player* player, float delta) {
	Vector2 playerCenter = {
	    player->pos.x + PLAYER_WIDTH / 2.0f,
	    player->pos.y + PLAYER_HEIGHT / 2.0f,
	};

	Vector2 enemyCenter = {
	    enemy->pos.x + enemy->stats.size / 2.0f,
	    enemy->pos.y + enemy->stats.size / 2.0f,
	};

	Vector2 direction = Vector2Subtract(playerCenter, enemyCenter);
	float length = Vector2Length(direction);

	if (length > 1.0f) {
		direction.x /= length;
		direction.y /= length;

		if (fabsf(direction.x) > fabsf(direction.y)) {
			enemy->direction = direction.x < 0.0f ? ENEMY_LEFT : ENEMY_RIGHT;
		} else {
			enemy->direction = direction.y < 0.0f ? ENEMY_UP : ENEMY_DOWN;
		}

		enemy->vel.x = direction.x * enemy->stats.speed;
		enemy->vel.y = direction.y * enemy->stats.speed;
	} else {
		enemy->vel = (Vector2){0.0f, 0.0f};
	}

	enemy->pos.x += enemy->vel.x * delta;
	enemy->pos.y += enemy->vel.y * delta;
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

void damageEnemy(Enemy* enemy, int amount) {
	if (enemy->invincibleTimer > 0.0f) return;

	enemy->health -= amount;
	enemy->invincibleTimer = ENEMY_INVINCIBLE_TIME;

	if (enemy->health <= 0) {
		enemy->active = false;
		enemy->health = 0;
		enemy->justDied = true;
	}
}