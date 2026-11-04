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
	for (int level = 0; level < PLAYER_MAX_LEVEL; level++) {
		snprintf(
		    path, sizeof(path), "resources/images/player/swordsman%d_idle.png",
		    level + 1);
		rm->playerIdleTextures[level] = LoadTexture(path);
		snprintf(
		    path, sizeof(path), "resources/images/player/swordsman%d_walk.png",
		    level + 1);
		rm->playerWalkTextures[level] = LoadTexture(path);
		snprintf(
		    path, sizeof(path),
		    "resources/images/player/swordsman%d_attack.png", level + 1);
		rm->playerAttackTextures[level] = LoadTexture(path);
		snprintf(
		    path, sizeof(path),
		    "resources/images/player/swordsman%d_walk_attack.png", level + 1);
		rm->playerWalkAttackTextures[level] = LoadTexture(path);
	}

	rm->groundTilesetTexture =
	    LoadTexture("resources/images/terrain/ground_tileset.png");
	rm->grassVarTexture = LoadTexture("resources/images/terrain/grass_var.png");
	rm->rockTexture = LoadTexture("resources/images/terrain/rocks.png");
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
	for (int level = 0; level < PLAYER_MAX_LEVEL; level++) {
		UnloadTexture(rm->playerIdleTextures[level]);
		UnloadTexture(rm->playerWalkTextures[level]);
		UnloadTexture(rm->playerAttackTextures[level]);
		UnloadTexture(rm->playerWalkAttackTextures[level]);
	}

	UnloadTexture(rm->groundTilesetTexture);
	UnloadTexture(rm->grassVarTexture);
	UnloadTexture(rm->rockTexture);
	UnloadTexture(rm->rockTexture);
	UnloadTexture(rm->healthPickupTexture);
	UnloadTexture(rm->xpGemTexture);

	UnloadFont(rm->uiFont);
	TraceLog(LOG_INFO, "Font valid: %d", IsFontValid(rm->uiFont));
	TraceLog(LOG_INFO, "Base size: %d", rm->uiFont.baseSize);
	TraceLog(LOG_INFO, "Glyph count: %d", rm->uiFont.glyphCount);
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
