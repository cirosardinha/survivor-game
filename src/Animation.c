#include "Animation.h"

void initAnimation(
    Animation* animation, Texture2D texture, float frameTime, bool loop) {
	animation->texture = texture;
	animation->frameCount = texture.width / FRAME_SIZE;
	animation->frameTime = frameTime;
	animation->loop = loop;
}

void setAnimation(AnimState* state, int newAnimation) {
	if (state->current == newAnimation) {
		return;
	};

	state->current = newAnimation;
	state->frame = 0;
	state->timer = 0.0f;
	state->finished = false;
}

void updateAnimation(
    const Animation* animation, AnimState* state, float delta) {
	if (state->finished) {
		return;
	}

	if (animation->frameTime <= 0.0f || animation->frameCount <= 0.0f) {
		return;
	}

	state->timer += delta;

	while (state->timer >= animation->frameTime) {
		state->timer -= animation->frameTime;
		state->frame++;

		if (state->frame >= animation->frameCount) {
			if (animation->loop) {
				state->frame = 0;
			} else {
				state->frame = animation->frameCount - 1;
				state->finished = true;
				return;
			}
		}
	}
}

void drawAnimation(
    const Animation* animation, const AnimState* state, Vector2 position,
    int direction) {
	if (state->finished) {
		return;
	}

	Rectangle src = {
	    .x = (float) state->frame * FRAME_SIZE,
	    .y = direction * FRAME_SIZE,
	    .width = FRAME_SIZE,
	    .height = FRAME_SIZE,
	};

	Rectangle dest = {
	    .x = position.x,
	    .y = position.y,
	    .width = FRAME_SIZE,
	    .height = FRAME_SIZE,
	};

	DrawTexturePro(
	    animation->texture, src, dest, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
}