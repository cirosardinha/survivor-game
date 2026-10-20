#include "Input.h"
#include "raymath.h"

void handleInput(Player* player) {
	player->vel = (Vector2){0.0f, 0.0f};

	if (IsKeyDown(KEY_A)) {
		player->vel.x = -1.0f;
		player->direction = PLAYER_LEFT;
	}

	if (IsKeyDown(KEY_D)) {
		player->vel.x = 1.0f;
		player->direction = PLAYER_RIGHT;
	}

	if (IsKeyDown(KEY_W)) {
		player->vel.y = -1.0f;
		player->direction = PLAYER_UP;
	}

	if (IsKeyDown(KEY_S)) {
		player->vel.y = 1.0f;
		player->direction = PLAYER_DOWN;
	}

	if (Vector2LengthSqr(player->vel) > 0.0f) {
		player->vel = Vector2Normalize(player->vel);
	}
}
