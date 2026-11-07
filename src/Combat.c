#include "Combat.h"
#include "Enemy.h"
#include "Player.h"
#include "config.h"
#include "raymath.h"

#define ATTACK_DETECT_RANGE (MAP_TILE_SIZE * 1.1f)
#define ATTACK_REACH 16.0f
#define ORB_DISTANCE 70.0f
#define ORB_RADIUS 14.0f
#define ORB_SPEED 4.0f
#define ORB_DAMAGE 10

static bool findAttackDirection(
    const Player* player, const Enemy* enemies, int count,
    PlayerDirection* outDirection);
static bool isPlayerInFront(
    const Enemy* enemy, Vector2 enemyCenter, Vector2 playerCenter,
    float distance);

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
	    player->pos.x + dir.x * ATTACK_REACH,
	    player->pos.y + dir.y * ATTACK_REACH,
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
				PlaySound(player->resources->playerAttackSound);

				attacked = true;
			}
			if (enemy->invincibleTimer <= 0.0f) {
				damageEnemy(enemy, player->attackDamage);
				if (enemy->active) PlaySound(enemy->resources->enemyHurtSound);
			}
		}
	}
}
void updateEnemyAttack(Enemy* enemy, Player* player, float delta) {
	if (!enemy->active) return;

	Vector2 enemyCenter = {
	    enemy->pos.x + enemy->stats.size / 2.0f,
	    enemy->pos.y + enemy->stats.size / 2.0f,
	};

	Vector2 playerCenter = {
	    player->pos.x + TILE_SIZE, player->pos.y + TILE_SIZE};

	float distance = Vector2Distance(enemyCenter, playerCenter);

	if (enemy->windupTimer > 0.0f) {
		enemy->windupTimer -= delta;

		if (enemy->windupTimer <= 0.0f) {
			enemy->windupTimer = 0.0f;

			float reach = enemy->stats.attackRange * ATTACK_HIT_TOLERANCE;

			if (distance <= reach &&
			    (enemy->stats.areaAttack ||
			     isPlayerInFront(enemy, enemyCenter, playerCenter, distance))) {
				damagePlayer(player, enemy->stats.attackDamage);
			}
		}
		return;
	}

	if (player->invincibleTimer > 0.0f) return;

	enemy->attackTimer += delta;

	if (distance <= enemy->stats.attackRange &&
	    enemy->attackTimer >= enemy->stats.attackCooldown) {
		startEnemyAttack(enemy);
		enemy->windupTimer = ATTACK_WINDUP_TIME;

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

static bool isPlayerInFront(
    const Enemy* enemy, Vector2 enemyCenter, Vector2 playerCenter,
    float distance) {
	Vector2 facing = getEnemyFacing(enemy);
	Vector2 toPlayer = Vector2Subtract(playerCenter, enemyCenter);
	float minDot = distance * cosf(ATTACK_CONE_ANGLE * DEG2RAD);

	return Vector2DotProduct(facing, toPlayer) >= minDot;
}

static Vector2 getOrbPosition(const Player* player) {
	return (Vector2){
	    player->pos.x + PLAYER_WIDTH / 2.0f +
	        cosf(player->orbAngle) * ORB_DISTANCE,
	    player->pos.y + PLAYER_HEIGHT / 2.0f +
	        sinf(player->orbAngle) * ORB_DISTANCE,
	};
}

void updateOrb(Player* player, Enemy* enemies, int count, float delta) {
	if (!player->hasOrb) return;

	player->orbAngle += ORB_SPEED * delta;

	Vector2 orbPos = getOrbPosition(player);

	for (int i = 0; i < count; i++) {
		Enemy* enemy = &enemies[i];
		if (!enemy->active) continue;

		Rectangle hitBox = {
		    enemy->pos.x,
		    enemy->pos.y,
		    enemy->stats.size,
		    enemy->stats.size,
		};

		if (CheckCollisionCircleRec(orbPos, ORB_RADIUS, hitBox)) {
			damageEnemy(enemy, ORB_DAMAGE);
		}
	}
}

void drawOrb(const Player* player) {
	if (!player->hasOrb) return;

	Vector2 pos = getOrbPosition(player);

	Rectangle source = {
	    0, 0, (float) player->resources->orbUpgradeTexture.width,
	    (float) player->resources->orbUpgradeTexture.height};

	Rectangle dest = {pos.x, pos.y, ORB_RADIUS * 2, ORB_RADIUS * 2};

	Vector2 origin = {ORB_RADIUS, ORB_RADIUS};

	DrawTexturePro(
	    player->resources->orbUpgradeTexture, source, dest, origin, 0.0f,
	    WHITE);
}