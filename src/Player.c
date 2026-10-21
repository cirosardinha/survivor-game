#include "Player.h"
#include "Enemy.h"
#include "config.h"

#define PLAYER_BLINK_INTERVAL 0.1f
#define PLAYER_ATTACK_DAMAGE 15

const float scale = 2.0f;
const float playerWidth = TILE_SIZE * scale;
const float playerHeight = TILE_SIZE * scale;

static const Vector2 playerAttackDir[4] = {
    {0, 1},
    {-1, 0},
    {1, 0},
    {0, -1},
};

static void clampPlayerToWorld(Player* player);
static void autoAttack(Player* player, Enemy* enemy, float delta);

void initPlayer(Player* player, Texture2D texture) {
	player->pos = (Vector2){
	    (WORLD_WIDTH * TILE_SIZE - playerWidth) / 2.0f,
	    (WORLD_HEIGHT * TILE_SIZE - playerHeight) / 2.0f,
	};

	player->vel = (Vector2){0, 0};

	player->health = 100;
	player->maxHealth = 100;
	player->speed = 200.0f;

	player->state = PLAYER_WALKING;
	player->direction = PLAYER_DOWN;

	player->attackTimer = 0.0f;
	player->attackCooldown = 2.0f;

	player->texture = texture;
	player->frame = 0;
	player->frameTimer = 0.0f;
	player->invincibleTimer = 0.0f;
}

void updatePlayer(Player* player, struct Enemy* enemy, float delta) {
	if (player->invincibleTimer > 0.0f) {
		player->invincibleTimer -= delta;
		if (player->invincibleTimer <= 0.0f) {
			player->invincibleTimer = 0.0f;
		}
	}
	player->frameTimer += delta;
	if (player->vel.x != 0.0f || player->vel.y != 0.0f) {
		if (player->frameTimer >= 0.15f) {
			player->frameTimer -= 0.15f;
			player->frame = (player->frame + 1) % 5;
		}
	} else {
		player->frame = 0;
		player->frameTimer = 0.0f;
	}
	player->pos.x += player->vel.x * player->speed * delta;
	player->pos.y += player->vel.y * player->speed * delta;

	clampPlayerToWorld(player);

	autoAttack(player, enemy, delta);
}

void drawPlayer(Player* const player) {
	float frameWidth = TILE_SIZE;
	float frameHeight = TILE_SIZE;
	int row = player->direction;

	Rectangle dest = {
	    player->pos.x,
	    player->pos.y,
	    frameWidth * scale,
	    frameHeight * scale,
	};

	Rectangle src = {
	    (float) player->frame * frameWidth,
	    row * frameHeight,
	    frameWidth,
	    frameHeight,
	};

	Color tint = WHITE;

	if (player->invincibleTimer > 0.0f) {
		int phase = (int) (player->invincibleTimer / PLAYER_BLINK_INTERVAL);
		if (phase % 2 == 0) {
			tint = Fade(RED, 0.3f);
		}
	}

	DrawTexturePro(
	    player->texture, src, dest, (Vector2){0.0f, 0.0f}, 0.0f, tint);
}

static void autoAttack(Player* player, Enemy* enemy, float delta) {
	Vector2 dir = playerAttackDir[player->direction];
	float areaX = (float) player->pos.x + dir.x * playerWidth;
	float areaY = (float) player->pos.y + dir.y * playerHeight;

	Rectangle playerAttackArea = {
	    areaX,
	    areaY,
	    playerWidth,
	    playerHeight,
	};

	Rectangle enemyHitBox = {
	    enemy->pos.x,
	    enemy->pos.y,
	    enemy->stats.size,
	    enemy->stats.size,
	};

	player->attackTimer += delta;

	if (player->attackTimer >= player->attackCooldown &&
	    CheckCollisionRecs(playerAttackArea, enemyHitBox)) {
		enemy->health -= PLAYER_ATTACK_DAMAGE;
		if (enemy->health < 0) {
			enemy->health = 0;
		}
		player->attackTimer = 0.0f;
		TraceLog(LOG_INFO, "Player attacked enemy!");
	}
}

void clampPlayerToWorld(Player* player) {
	float worldWidth = WORLD_WIDTH * TILE_SIZE;
	float worldHeight = WORLD_HEIGHT * TILE_SIZE;

	if (player->pos.x < 0) {
		player->pos.x = 0;
	} else if (player->pos.x > worldWidth - playerWidth) {
		player->pos.x = worldWidth - playerWidth;
	}

	if (player->pos.y < 0) {
		player->pos.y = 0;
	} else if (player->pos.y > worldHeight - playerHeight) {
		player->pos.y = worldHeight - playerHeight;
	}
}