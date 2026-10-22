#include "Collision.h"
#include "config.h"
#include "raylib.h"
#include "raymath.h"

void resolveEnemyPlayerCollision(Enemy* enemy, Player* player) {
	if (!enemy->active) return;

	float playerSize = TILE_SIZE * 2.0f;

	Rectangle playerRect = {
	    player->pos.x,
	    player->pos.y,
	    playerSize,
	    playerSize,
	};

	Rectangle enemyRect = {
	    enemy->pos.x,
	    enemy->pos.y,
	    enemy->stats.size,
	    enemy->stats.size,
	};

	if (!CheckCollisionRecs(playerRect, enemyRect)) return;

	Vector2 direction = {
	    enemy->pos.x + enemy->stats.size / 2.0f -
	        (player->pos.x + playerSize / 2.0f),
	    enemy->pos.y + enemy->stats.size / 2.0f -
	        (player->pos.y + playerSize / 2.0f),
	};

	float length = Vector2Length(direction);

	if (length == 0.0f) return;

	direction.x /= length;
	direction.y /= length;

	float pushDistance = 2.0f;

	enemy->pos.x += direction.x * pushDistance;
	enemy->pos.y += direction.y * pushDistance;
}