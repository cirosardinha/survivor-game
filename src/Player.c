#include "Player.h"
#include "config.h"

#define PLAYER_BLINK_INTERVAL 0.1f
#define PLAYER_INVINCIBLE_TIME 0.5f

#define PLAYER_IDLE_FRAME_TIME 0.12f
#define PLAYER_WALK_FRAME_TIME 0.10F
#define PLAYER_ATTACK_FRAME_TIME 0.05f
#define PLAYER_WALK_ATTACK_FRAME_TIME 0.05f

#define XP_STEP 50
#define SPRITE_TIER_2_LEVEL 5
#define SPRITE_TIER_3_LEVEL 10

static void clampPlayerToWorld(Player* player);
static bool isPlayerMoving(const Player* player);
static void setupPlayerAnimations(Player* player);
static void levelUpPlayer(Player* player);

static int xpToReachLevel(int level);
static int spriteTier(int level);

void initPlayer(Player* player, ResourceManager* rm) {
	player->pos = (Vector2){
	    (WORLD_WIDTH * MAP_TILE_SIZE - PLAYER_WIDTH) / 2.0f,
	    (WORLD_HEIGHT * MAP_TILE_SIZE - PLAYER_HEIGHT) / 2.0f,
	};

	player->vel = (Vector2){0, 0};

	player->health = 100;
	player->maxHealth = 100;
	player->speed = 210.0f;
	player->score = 0;
	player->xp = 0;
	player->level = 1;
	player->attackDamage = 16;
	player->pickupRange = PICKUP_BASE_RANGE;
	player->pendingLevelUps = 0;

	player->resources = rm;

	player->state = PLAYER_NORMAL;
	player->direction = PLAYER_DOWN;

	player->attackTimer = 0.0f;
	player->attackCooldown = 1.0f;
	player->invincibleTimer = 0.0f;

	setupPlayerAnimations(player);

	player->animState = (AnimState){
	    .current = PLAYER_ANIM_IDLE,
	};
}
void updatePlayer(Player* player, float delta) {
	if (player->invincibleTimer > 0.0f) {
		player->invincibleTimer -= delta;
		if (player->invincibleTimer <= 0.0f) {
			player->invincibleTimer = 0.0f;
		}
	}

	if (player->state == PLAYER_ATTACKING && player->animState.finished) {
		player->state = PLAYER_NORMAL;
	}

	if (player->state == PLAYER_NORMAL) {
		PlayerAnim anim =
		    isPlayerMoving(player) ? PLAYER_ANIM_WALK : PLAYER_ANIM_IDLE;
		setAnimation(&player->animState, anim);
	}

	updateAnimation(
	    &player->anims[player->animState.current], &player->animState, delta);

	player->pos.x += player->vel.x * player->speed * delta;
	player->pos.y += player->vel.y * player->speed * delta;

	clampPlayerToWorld(player);
}

void drawPlayer(Player* const player) {
	Color tint = WHITE;

	if (player->invincibleTimer > 0.0f) {
		int phase = (int) (player->invincibleTimer / PLAYER_BLINK_INTERVAL);
		if (phase % 2 == 0) {
			tint = Fade(RED, 0.3f);
		}
	}

	Vector2 spritePos = {
	    player->pos.x + PLAYER_WIDTH / 2.0f - SPRITE_SIZE / 2.0f,
	    player->pos.y + PLAYER_HEIGHT / 2.0f - SPRITE_SIZE / 2.0f,
	};

	drawAnimation(
	    &player->anims[player->animState.current], &player->animState,
	    spritePos, player->direction, tint);
}

void startPlayerAttack(Player* player) {
	player->state = PLAYER_ATTACKING;

	PlayerAnim anim =
	    isPlayerMoving(player) ? PLAYER_ANIM_WALK_ATTACK : PLAYER_ANIM_ATTACK;
	setAnimation(&player->animState, anim);
}

void damagePlayer(Player* player, int amount) {
	if (player->invincibleTimer > 0.0f) return;

	player->health -= amount;
	player->invincibleTimer = PLAYER_INVINCIBLE_TIME;

	PlaySound(player->resources->playerHurtSound);
}

static bool isPlayerMoving(const Player* player) {
	return player->vel.x != 0.0f || player->vel.y != 0.0f;
}

static void clampPlayerToWorld(Player* player) {
	float worldWidth = WORLD_WIDTH * MAP_TILE_SIZE;
	float worldHeight = WORLD_HEIGHT * MAP_TILE_SIZE;

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

void addPlayerXp(Player* player, int amount) {
	player->xp += amount;

	while (player->level < PLAYER_MAX_LEVEL &&
	       player->xp >= xpToReachLevel(player->level + 1)) {
		levelUpPlayer(player);
	}
}

float getPlayerXpProgress(const Player* player) {
	if (player->level >= PLAYER_MAX_LEVEL) return 1.0f;

	int previous = xpToReachLevel(player->level);
	int next = xpToReachLevel(player->level + 1);

	return (float) (player->xp - previous) / (float) (next - previous);
}

static void setupPlayerAnimations(Player* player) {
	const ResourceManager* rm = player->resources;
	int i = spriteTier(player->level);

	initAnimation(
	    &player->anims[PLAYER_ANIM_IDLE], rm->playerIdleTextures[i],
	    PLAYER_IDLE_FRAME_TIME, true);
	initAnimation(
	    &player->anims[PLAYER_ANIM_WALK], rm->playerWalkTextures[i],
	    PLAYER_WALK_FRAME_TIME, true);
	initAnimation(
	    &player->anims[PLAYER_ANIM_ATTACK], rm->playerAttackTextures[i],
	    PLAYER_ATTACK_FRAME_TIME, false);
	initAnimation(
	    &player->anims[PLAYER_ANIM_WALK_ATTACK],
	    rm->playerWalkAttackTextures[i], PLAYER_WALK_ATTACK_FRAME_TIME, false);

	setAnimationRowFrames(&player->anims[PLAYER_ANIM_IDLE], PLAYER_UP, 4);
}

static void levelUpPlayer(Player* player) {
	int previousTier = spriteTier(player->level);

	player->level++;
	player->pendingLevelUps++;

	if (spriteTier(player->level) != previousTier) {
		setupPlayerAnimations(player);
	}
}

static int xpToReachLevel(int level) {
	return XP_STEP * level * (level - 1) / 2;
}

static int spriteTier(int level) {
	if (level >= SPRITE_TIER_3_LEVEL) return 2;
	if (level >= SPRITE_TIER_2_LEVEL) return 1;
	return 0;
}