#include <stdio.h>
#include <stdlib.h>

#include "ResourceManager.h"

ResourceManager* createResourceManager(void) {
	ResourceManager* rm = malloc(sizeof(ResourceManager));
	if (rm == NULL) exit(EXIT_FAILURE);
	rm->loaded = 0;
	return rm;
}

void loadResources(ResourceManager* rm) {
	char path[96];

	for (int tier = 0; tier < PLAYER_SPRITE_TIERS; tier++) {
		snprintf(
		    path, sizeof(path), "resources/images/player/swordsman%d_idle.png",
		    tier + 1);
		rm->playerIdleTextures[tier] = LoadTexture(path);
		snprintf(
		    path, sizeof(path), "resources/images/player/swordsman%d_walk.png",
		    tier + 1);
		rm->playerWalkTextures[tier] = LoadTexture(path);
		snprintf(
		    path, sizeof(path),
		    "resources/images/player/swordsman%d_attack.png", tier + 1);
		rm->playerAttackTextures[tier] = LoadTexture(path);
		snprintf(
		    path, sizeof(path),
		    "resources/images/player/swordsman%d_walk_attack.png", tier + 1);
		rm->playerWalkAttackTextures[tier] = LoadTexture(path);
	}

	rm->groundTilesetTexture =
	    LoadTexture("resources/images/terrain/ground_tileset.png");
	rm->grassVarTexture = LoadTexture("resources/images/terrain/grass_var.png");
	rm->rockTexture = LoadTexture("resources/images/terrain/rocks.png");
	rm->healthPickupTexture =
	    LoadTexture("resources/images/pickups/health_pickup.png");
	rm->xpGemTexture = LoadTexture("resources/images/pickups/xp_gem.png");

	rm->uiFont = LoadFont("resources/fonts/pixel_font.fnt");
	rm->hpFrameTexture = LoadTexture("resources/images/ui/hp_frame.png");
	rm->hpFillTexture = LoadTexture("resources/images/ui/hp_fill.png");
	rm->xpFrameTexture = LoadTexture("resources/images/ui/xp_frame.png");
	rm->xpFillTexture = LoadTexture("resources/images/ui/xp_fill.png");
	rm->scoreFrameTexture = LoadTexture("resources/images/ui/score_frame.png");
	rm->timerFrameTexture = LoadTexture("resources/images/ui/timer_frame.png");
	rm->enemyHpBackTexture =
	    LoadTexture("resources/images/ui/enemy_hp_back.png");
	rm->enemyHpFillTexture =
	    LoadTexture("resources/images/ui/enemy_hp_fill.png");

	for (int type = 0; type < ENEMY_TYPE_COUNT; type++) {
		const char* name = getEnemyStats(type)->name;

		for (int variant = 0; variant < ENEMY_VARIANTS; variant++) {
			snprintf(
			    path, sizeof(path), "resources/images/enemies/%s%d_walk.png",
			    name, variant + 1);
			rm->enemyWalkTextures[type][variant] = LoadTexture(path);
			snprintf(
			    path, sizeof(path), "resources/images/enemies/%s%d_attack.png",
			    name, variant + 1);
			rm->enemyAttackTextures[type][variant] = LoadTexture(path);
		}

		// rm->jumpSound = LoadSound("resources/sfx/jump.wav");
		// rm->backgroundMusic = LoadMusicStream("resources/musics/theme.ogg");
		rm->loaded = 1;
	}
}

void unloadResources(ResourceManager* rm) {
	if (!rm->loaded) return;
	for (int tier = 0; tier < PLAYER_SPRITE_TIERS; tier++) {
		UnloadTexture(rm->playerIdleTextures[tier]);
		UnloadTexture(rm->playerWalkTextures[tier]);
		UnloadTexture(rm->playerAttackTextures[tier]);
		UnloadTexture(rm->playerWalkAttackTextures[tier]);
	}

	UnloadTexture(rm->groundTilesetTexture);
	UnloadTexture(rm->grassVarTexture);
	UnloadTexture(rm->rockTexture);
	UnloadTexture(rm->healthPickupTexture);
	UnloadTexture(rm->xpGemTexture);

	UnloadFont(rm->uiFont);
	UnloadTexture(rm->hpFrameTexture);
	UnloadTexture(rm->hpFillTexture);
	UnloadTexture(rm->xpFrameTexture);
	UnloadTexture(rm->xpFillTexture);
	UnloadTexture(rm->scoreFrameTexture);
	UnloadTexture(rm->timerFrameTexture);
	UnloadTexture(rm->enemyHpBackTexture);
	UnloadTexture(rm->enemyHpFillTexture);

	for (int type = 0; type < ENEMY_TYPE_COUNT; type++) {
		for (int variant = 0; variant < ENEMY_VARIANTS; variant++) {
			UnloadTexture(rm->enemyWalkTextures[type][variant]);
			UnloadTexture(rm->enemyAttackTextures[type][variant]);
		}
	}
	// UnloadSound(rm->jumpSound);
	// UnloadMusicStream(rm->backgroundMusic);

	rm->loaded = 0;
}

void destroyResourceManager(ResourceManager* rm) {
	unloadResources(rm);
	free(rm);
}
