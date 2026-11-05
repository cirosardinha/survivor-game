#include "Combat.h"
#include "Enemy.h"
#include "Player.h"
#include "config.h"
#include "raymath.h"

#define ATTACK_DETECT_RANGE (MAP_TILE_SIZE * 1.25f)

static bool findAttackDirection(
    const Player* player, const Enemy* enemies, int count,
    PlayerDirection* outDirection);

static const Vector2 playerAttackDir[4] = {
    {0, 1},
    {-1, 0},
    {1, 0},
    {0, -1},
};

void updatePlayerAttack(
    Player* player, Enemy* enemies, int count, float delta) {
	player->attackTimer += delta;
	if (player->attackTimer < player->attackCooldown) return;

	PlayerDirection attackDirection;
	if (!findAttackDirection(player, enemies, count, &attackDirection)) {
		return;
	}

	float playerWidth = TILE_SIZE * 2.0f;
	float playerHeight = TILE_SIZE * 2.0f;

	Vector2 dir = playerAttackDir[attackDirection];

	Rectangle playerAttackArea = {
	    player->pos.x + dir.x * playerWidth,
	    player->pos.y + dir.y * playerHeight,
	    playerWidth,
	    playerHeight,
	};

	bool attacked = false;

	for (int i = 0; i < count; i++) {
		Enemy* enemy = &enemies[i];

		if (!enemy->active) continue;

		Rectangle enemyHitBox = {
		    enemy->pos.x,
		    enemy->pos.y,
		    enemy->stats.size,
		    enemy->stats.size,
		};

		if (CheckCollisionRecs(playerAttackArea, enemyHitBox)) {
			if (!attacked) {
				player->direction = attackDirection;
				startPlayerAttack(player);
				player->attackTimer = 0.0f;
				attacked = true;
			}
			damageEnemy(enemy, player->attackDamage);
		}
	}
}

void updateEnemyAttack(Enemy* enemy, Player* player, float delta) {
	if (!enemy->active) return;
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
		startEnemyAttack(enemy);
		damagePlayer(player, enemy->stats.attackDamage);

		enemy->attackTimer = 0.0f;
	}
}

static bool findAttackDirection(
    const Player* player, const Enemy* enemies, int count,
    PlayerDirection* outDirection) {
	Vector2 playerCenter = {
	    player->pos.x + PLAYER_WIDTH / 2.0f,
	    player->pos.y + PLAYER_HEIGHT / 2.0f,
	};

	float bestDistSq = ATTACK_DETECT_RANGE * ATTACK_DETECT_RANGE;
	Vector2 bestOffset = {0.0f, 0.0f};
	bool found = false;

	for (int i = 0; i < count; i++) {
		const Enemy* enemy = &enemies[i];
		if (!enemy->active) continue;

		float dx = enemy->pos.x + enemy->stats.size / 2.0f - playerCenter.x;
		float dy = enemy->pos.y + enemy->stats.size / 2.0f - playerCenter.y;
		float distSq = dx * dx + dy * dy;

		if (distSq < bestDistSq) {
			bestDistSq = distSq;
			bestOffset = (Vector2){dx, dy};
			found = true;
		}
	}

	if (!found) return false;

	if (fabsf(bestOffset.x) > fabsf(bestOffset.y)) {
		*outDirection = bestOffset.x < 0.0f ? PLAYER_LEFT : PLAYER_RIGHT;
	} else {
		*outDirection = bestOffset.y < 0.0f ? PLAYER_UP : PLAYER_DOWN;
	}

	return true;
}