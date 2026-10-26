#pragma once

#include "raylib.h"
#include <stdbool.h>

#define FRAME_SIZE 64

typedef struct Animation {
	Texture2D texture;
	int frameCount;
	float frameTime;
	bool loop;
} Animation;

typedef struct AnimState {
	int current;
	int frame;
	float timer;
	bool finished;
} AnimState;

void initAnimation(
    Animation* animation, Texture2D texture, float frameTime, bool loop);
void setAnimation(AnimState* state, int newAnimation);
void updateAnimation(const Animation* animation, AnimState* state, float delta);
void drawAnimation(
    const Animation* animation, const AnimState* state, Vector2 position,
    int direction);
