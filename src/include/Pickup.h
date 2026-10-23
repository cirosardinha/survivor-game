#pragma once

#include "Player.h"
#include "raylib.h"
#include <stdbool.h>

typedef enum PickupType {
	PICKUP_HEALTH,
	PICKUP_XP,
	PICKUP_TYPE_COUNT,
} PickupType;

typedef struct Pickup {
	bool active;
	PickupType type;
	Vector2 pos;
	int value;
	float lifetime;

	Texture2D texture;
	int frame;
	float frameTimer;
} Pickup;

void initPickup(
    Pickup* pickup, Texture2D texture, PickupType type, Vector2 pos);
void updatePickup(Pickup* pickup, Player* player, float delta);
void drawPickup(const Pickup* pickup);