#include "Collision.h"
#include "Map.h"
#include "config.h"
#include "raylib.h"
#include "raymath.h"

#define BODY_SCALE 0.5f
#define PUSH_STRENGTH 20.0f
#define ENEMY_BODY_SCALE 0.7f
#define ENEMY_OBSTACLE_BODY_SCALE 0.5f
#define ENEMY_PUSH_STRENGTH 60.0f

static void
resolveEnemyEnemyCollision(Enemy* firstEnemy, Enemy* secondEnemy, float delta);

void resolveEnemyPlayerCollision(Enemy* enemy, Player* player, float delta) {
	if (!enemy->active) return;

	Vector2 playerCenter = {
	    player->pos.x + PLAYER_WIDTH / 2.0f,
	    player->pos.y + PLAYER_HEIGHT / 2.0f,
	};
	Vector2 enemyCenter = {
	    enemy->pos.x + enemy->stats.size / 2.0f,
	    enemy->pos.y + enemy->stats.size / 2.0f,
	};

	float playerBodySize = PLAYER_WIDTH * BODY_SCALE;
	float enemyBodySize = enemy->stats.size * BODY_SCALE;

	Rectangle playerRect = {
	    playerCenter.x - playerBodySize / 2.0f,
	    playerCenter.y - playerBodySize / 2.0f,
	    playerBodySize,
	    playerBodySize,
	};
	Rectangle enemyRect = {
	    enemyCenter.x - enemyBodySize / 2.0f,
	    enemyCenter.y - enemyBodySize / 2.0f,
	    enemyBodySize,
	    enemyBodySize,
	};

	Rectangle overlapRect = GetCollisionRec(playerRect, enemyRect);
	if (overlapRect.width <= 0.0f || overlapRect.height <= 0.0f) return;

	Vector2 direction = Vector2Subtract(enemyCenter, playerCenter);
	float length = Vector2Length(direction);

	if (length < 0.001f) {
		direction = (Vector2){1.0f, 0.0f};
	} else {
		direction.x /= length;
		direction.y /= length;
	}

	float penetration = fminf(overlapRect.width, overlapRect.height);
	float pushFactor = fminf(1.0f, PUSH_STRENGTH * delta);

	enemy->pos.x += direction.x * penetration * pushFactor;
	enemy->pos.y += direction.y * penetration * pushFactor;
}

static void
resolveEnemyEnemyCollision(Enemy* firstEnemy, Enemy* secondEnemy, float delta) {
	if (!firstEnemy->active || !secondEnemy->active) return;

	Vector2 firstCenter = {
	    firstEnemy->pos.x + firstEnemy->stats.size / 2.0f,
	    firstEnemy->pos.y + firstEnemy->stats.size / 2.0f,
	};
	Vector2 secondCenter = {
	    secondEnemy->pos.x + secondEnemy->stats.size / 2.0f,
	    secondEnemy->pos.y + secondEnemy->stats.size / 2.0f,
	};

	float firstBodySize = firstEnemy->stats.size * ENEMY_BODY_SCALE;
	float secondBodySize = secondEnemy->stats.size * ENEMY_BODY_SCALE;

	Rectangle firstBodyRect = {
	    firstCenter.x - firstBodySize / 2.0f,
	    firstCenter.y - firstBodySize / 2.0f,
	    firstBodySize,
	    firstBodySize,
	};
	Rectangle secondBodyRect = {
	    secondCenter.x - secondBodySize / 2.0f,
	    secondCenter.y - secondBodySize / 2.0f,
	    secondBodySize,
	    secondBodySize,
	};

	Rectangle overlapRect = GetCollisionRec(firstBodyRect, secondBodyRect);
	if (overlapRect.width <= 0.0f || overlapRect.height <= 0.0f) return;

	Vector2 direction = Vector2Subtract(secondCenter, firstCenter);
	float length = Vector2Length(direction);

	if (length < 0.001f) {
		direction = (Vector2){1.0f, 0.0f};
	} else {
		direction.x /= length;
		direction.y /= length;
	}

	float penetration = fminf(overlapRect.width, overlapRect.height);
	float pushFactor = fminf(1.0f, ENEMY_PUSH_STRENGTH * delta);
	float pushDistance = penetration * pushFactor * 0.5f;

	firstEnemy->pos.x -= direction.x * pushDistance;
	firstEnemy->pos.y -= direction.y * pushDistance;
	secondEnemy->pos.x += direction.x * pushDistance;
	secondEnemy->pos.y += direction.y * pushDistance;
}

void resolveAllEnemiesCollision(Enemy* enemies, int enemyCount, float delta) {
	for (int enemyIndex = 0; enemyIndex < enemyCount; enemyIndex++) {
		for (int otherEnemyIndex = enemyIndex + 1; otherEnemyIndex < enemyCount;
		     otherEnemyIndex++) {
			resolveEnemyEnemyCollision(
			    &enemies[enemyIndex], &enemies[otherEnemyIndex], delta);
		}
	}
}

static Rectangle getPlayerBody(const Player* player) {
	float size = PLAYER_WIDTH * BODY_SCALE;
	return (Rectangle){
	    player->pos.x + (PLAYER_WIDTH - size) / 2.0f,
	    player->pos.y + (PLAYER_HEIGHT - size) / 2.0f,
	    size,
	    size,
	};
}

void resolvePlayerObstacleCollision(Player* player, Vector2 previousPos) {
	Vector2 newPos = player->pos;

	player->pos = (Vector2){newPos.x, previousPos.y};
	if (isMapRectBlocked(getPlayerBody(player))) {
		newPos.x = previousPos.x;
	}

	player->pos = newPos;
	if (isMapRectBlocked(getPlayerBody(player))) {
		newPos.y = previousPos.y;
	}

	player->pos = newPos;
}

static Rectangle getEnemyBody(const Enemy* enemy) {
	float size = enemy->stats.size * ENEMY_OBSTACLE_BODY_SCALE;
	float offset = (enemy->stats.size - size) / 2.0f;
	return (Rectangle){
	    enemy->pos.x + offset,
	    enemy->pos.y + offset,
	    size,
	    size,
	};
}

void resolveEnemyObstacleCollision(Enemy* enemy, Vector2 previousPos) {
	if (!enemy->active) return;

	Vector2 newPos = enemy->pos;

	enemy->pos = (Vector2){newPos.x, previousPos.y};
	if (isMapRectBlocked(getEnemyBody(enemy))) {
		newPos.x = previousPos.x;
	}

	enemy->pos = newPos;
	if (isMapRectBlocked(getEnemyBody(enemy))) {
		newPos.y = previousPos.y;
	}

	enemy->pos = newPos;
}