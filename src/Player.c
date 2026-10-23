#include "Player.h"
#include "config.h"

#define PLAYER_BLINK_INTERVAL 0.1f
#define PLAYER_INVINCIBLE_TIME 0.5f
#define PLAYER_ATTACK_INTERVAL 0.1f

static void clampPlayerToWorld(Player* player);

void initPlayer(Player* player, Texture2D texture) {
	player->pos = (Vector2){
	    (WORLD_WIDTH * TILE_SIZE - PLAYER_WIDTH) / 2.0f,
	    (WORLD_HEIGHT * TILE_SIZE - PLAYER_HEIGHT) / 2.0f,
	};

	player->vel = (Vector2){0, 0};

	player->health = 100;
	player->maxHealth = 100;
	player->speed = 250.0f;
	player->score = 0;

	player->state = PLAYER_WALKING;
	player->direction = PLAYER_DOWN;

	player->attackTimer = 0.0f;
	player->attackCooldown = 1.2f;

	player->texture = texture;
	player->frame = 0;
	player->frameTimer = 0.0f;
	player->invincibleTimer = 0.0f;
}

void updatePlayer(Player* player, float delta) {
	if (player->invincibleTimer > 0.0f) {
		player->invincibleTimer -= delta;
		if (player->invincibleTimer <= 0.0f) {
			player->invincibleTimer = 0.0f;
		}
	}
	if (player->state == PLAYER_ATTACKING) {
		player->frameTimer += delta;
		if (player->frameTimer >= PLAYER_ATTACK_INTERVAL) {
			player->frameTimer -= PLAYER_ATTACK_INTERVAL;
			player->frame++;
			if (player->frame >= 4) {
				player->state = PLAYER_WALKING;
				player->frame = 0;
				player->frameTimer = 0.0f;
			}
		}
	} else {
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
	}
	player->pos.x += player->vel.x * player->speed * delta;
	player->pos.y += player->vel.y * player->speed * delta;

	clampPlayerToWorld(player);
}

void drawPlayer(Player* const player) {
	float frameWidth = TILE_SIZE;
	float frameHeight = TILE_SIZE;
	int row = player->direction;

	Rectangle dest = {
	    player->pos.x,
	    player->pos.y,
	    frameWidth * SCALE,
	    frameHeight * SCALE,
	};

	int column = player->frame;
	if (player->state == PLAYER_ATTACKING) {
		column += 5;
	}

	Rectangle src = {
	    column * frameWidth,
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

void damagePlayer(Player* player, int amount) {
	if (player->invincibleTimer > 0.0f) return;

	player->health -= amount;
	player->invincibleTimer = PLAYER_INVINCIBLE_TIME;
}

void clampPlayerToWorld(Player* player) {
	float worldWidth = WORLD_WIDTH * TILE_SIZE;
	float worldHeight = WORLD_HEIGHT * TILE_SIZE;

	if (player->pos.x < 0) {
		player->pos.x = 0;
	} else if (player->pos.x > worldWidth - PLAYER_WIDTH) {
		player->pos.x = worldWidth - PLAYER_WIDTH;
	}

	if (player->pos.y < 0) {
		player->pos.y = 0;
	} else if (player->pos.y > worldHeight - PLAYER_HEIGHT) {
		player->pos.y = worldHeight - PLAYER_HEIGHT;
	}
}

void healPlayer(Player* player, int amount) {
	if (player->health >= player->maxHealth) return;

	player->health += amount;
	if (player->health > player->maxHealth) {
		player->health = player->maxHealth;
	}
}