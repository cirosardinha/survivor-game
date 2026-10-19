#include "Player.h"
#include "config.h"

void initPlayer(Player* player) {
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
}

void updatePlayer(Player* player, float delta) {
	player->pos.x += player->vel.x * player->speed * delta;
	player->pos.y += player->vel.y * player->speed * delta;
}

void drawPlayer(Player* const player) {
	DrawRectangle(player->pos.x, player->pos.y, TILE_SIZE, TILE_SIZE, BLUE);
}