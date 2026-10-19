#include "Player.h"
#include "config.h"

void initPlayer(Player* player, Texture2D texture) {
	player->pos = (Vector2){
	    WORLD_WIDTH * TILE_SIZE / 2.0f,
	    WORLD_HEIGHT * TILE_SIZE / 2.0f,
	};

	player->vel = (Vector2){0, 0};

	player->health = 100;
	player->maxHealth = 100;
	player->speed = 150.0f;

	player->state = PLAYER_WALKING;
	player->direction = PLAYER_DOWN;

	player->texture = texture;
	player->frame = 0;
	player->frameTimer = 0.0f;
}

void updatePlayer(Player* player, float delta) {
	player->frameTimer += delta;
	if (player->frameTimer >= 0.15f) {
		player->frameTimer -= 0.15f;
		player->frame++;

		if (player->frame > 3) {
			player->frame = 0;
		}

		if (player->vel.x == 0.0f && player->vel.y == 0.0f) {
			player->frame = 0;
		}
	}
	player->pos.x += player->vel.x * player->speed * delta;
	player->pos.y += player->vel.y * player->speed * delta;

	float worldWidth = WORLD_WIDTH * TILE_SIZE;
	float worldHeight = WORLD_HEIGHT * TILE_SIZE;
	float playerWidth = player->texture.width / 4.0f;
	float playerHeight = player->texture.width / 4.0f;

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

void drawPlayer(Player* const player) {
	float frameWidth = TILE_SIZE;
	float frameHeight = TILE_SIZE;
	int row = player->direction;

	float scale = 2.0f;

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

	DrawTexturePro(
	    player->texture, src, dest, (Vector2){0.0f, 0.0f}, 0.0f, tint);
}