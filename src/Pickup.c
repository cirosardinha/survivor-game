#include "Pickup.h"
#include "config.h"
#include "raymath.h"

#define PICKUP_FRAME_SIZE 16.0f
#define PICKUP_SIZE (PICKUP_FRAME_SIZE * SCALE)
#define PICKUP_COLLECT_RANGE PICKUP_SIZE
#define PICKUP_LIFE_TIME 10.0f
#define PICKUP_FRAME_COUNT 4

void initPickup(
    Pickup* pickup, Texture2D texture, PickupType type, Vector2 pos) {
	pickup->active = true;
	pickup->type = type;
	pickup->pos = pos;
	pickup->lifetime = 0.0f;
	pickup->texture = texture;
	pickup->frame = 0;
	pickup->frameTimer = 0.0f;

	switch (type) {
		case PICKUP_HEALTH:
			pickup->value = 10;
			break;
		case PICKUP_XP:
			pickup->value = 100;
			break;

		default:
			pickup->value = 0;
			break;
	}
}

void updatePickup(Pickup* pickup, Player* player, float delta) {
	if (!pickup->active) return;

	pickup->frameTimer += delta;
	if (pickup->frameTimer >= 0.15f) {
		pickup->frameTimer -= 0.15f;
		pickup->frame = (pickup->frame + 1) % PICKUP_FRAME_COUNT;
	}
	pickup->lifetime += delta;
	if (pickup->lifetime > PICKUP_LIFE_TIME) {
		pickup->active = false;
		return;
	}

	Vector2 playerCenter = {
	    player->pos.x + PLAYER_WIDTH / 2.0f,
	    player->pos.y + PLAYER_HEIGHT / 2.0f,
	};

	float range = PICKUP_COLLECT_RANGE;
	bool inRange =
	    Vector2DistanceSqr(playerCenter, pickup->pos) < range * range;

	if (!inRange) return;

	switch (pickup->type) {
		case PICKUP_HEALTH:
			if (player->health >= player->maxHealth) return;
			healPlayer(player, pickup->value);
			pickup->active = false;
			break;

		case PICKUP_XP:
			player->score += pickup->value;
			pickup->active = false;
			break;

		default:
			break;
	}
}

void drawPickup(const Pickup* pickup) {
	if (!pickup->active) return;

	Rectangle src = {
	    pickup->frame * PICKUP_FRAME_SIZE,
	    0.0f,
	    PICKUP_FRAME_SIZE,
	    PICKUP_FRAME_SIZE,
	};
	Rectangle dest = {
	    pickup->pos.x - PICKUP_SIZE / 2.0f,
	    pickup->pos.y - PICKUP_SIZE / 2.0f,
	    PICKUP_SIZE,
	    PICKUP_SIZE,
	};

	DrawTexturePro(
	    pickup->texture, src, dest, (Vector2){0.f, 0.0f}, 0.0f, WHITE);
}