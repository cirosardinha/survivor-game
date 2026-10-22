#include "Combat.h"
#include "Enemy.h"
#include "Player.h"
#include "config.h"
#include "raymath.h"

#define PLAYER_ATTACK_DAMAGE 15

static const Vector2 playerAttackDir[4] = {
    {0, 1},
    {-1, 0},
    {1, 0},
    {0, -1},
};

void updatePlayerAttack(
    Player* player, Enemy* enemies, int count, float delta) {
	bool isCooldownOver = player->attackTimer >= player->attackCooldown;
	player->attackTimer += delta;

	if (!isCooldownOver) return;

	player->state = PLAYER_ATTACKING;
	player->frame = 0;
	player->frameTimer = 0.0f;
	player->attackTimer = 0.0f;

	float playerWidth = TILE_SIZE * 2.0f;
	float playerHeight = TILE_SIZE * 2.0f;

	Vector2 dir = playerAttackDir[player->direction];

	Rectangle playerAttackArea = {
	    player->pos.x + dir.x * playerWidth,
	    player->pos.y + dir.y * playerHeight,
	    playerWidth,
	    playerHeight,
	};

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
			damageEnemy(enemy, PLAYER_ATTACK_DAMAGE);
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
		damagePlayer(player, enemy->stats.attackDamage);

		enemy->attackTimer = 0.0f;
	}
}
