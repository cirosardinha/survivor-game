#include "Animation.h"

void initAnimation(
    Animation* animation, Texture2D texture, float frameTime, bool loop) {
	animation->texture = texture;
	animation->frameCount = texture.width / FRAME_SIZE;
	animation->frameTime = frameTime;
	animation->loop = loop;

	for (int i = 0; i < ANIM_DIRECTIONS; i++) {
		animation->rowFrameCount[i] = animation->frameCount;
	}
}

void setAnimationRowFrames(Animation* animation, int row, int count) {
	if (row < 0 || row >= ANIM_DIRECTIONS) {
		return;
	}
	animation->rowFrameCount[row] = count;
}

void setAnimation(AnimState* state, int newAnimation) {
	if (state->current == newAnimation) {
		return;
	}

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

	if (animation->frameTime <= 0.0f || animation->frameCount <= 0) {
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
    int direction, Color tint) {
	if (direction < 0 || direction >= ANIM_DIRECTIONS) {
		direction = 0;
	}

	int rowFrames = animation->rowFrameCount[direction];
	if (rowFrames == 0) {
		rowFrames = 1;
	}

	int frame = state->frame % rowFrames;

	Rectangle src = {
	    .x = (float) frame * FRAME_SIZE,
	    .y = (float) direction * FRAME_SIZE,
	    .width = FRAME_SIZE,
	    .height = FRAME_SIZE,
	};

	Rectangle dest = {
	    .x = position.x,
	    .y = position.y,
	    .width = SPRITE_SIZE,
	    .height = SPRITE_SIZE,
	};

	DrawTexturePro(
	    animation->texture, src, dest, (Vector2){0.0f, 0.0f}, 0.0f, tint);
}