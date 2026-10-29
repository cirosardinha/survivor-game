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

	rm->grassTexture = LoadTexture("resources/images/terrain/grass.png");
	rm->grassVarTexture = LoadTexture("resources/images/terrain/grass_var.png");
	rm->bushTexture = LoadTexture("resources/images/terrain/bush.png");
	rm->dirtTexture = LoadTexture("resources/images/terrain/dirt.png");
	rm->rockTexture = LoadTexture("resources/images/terrain/rocks.png");
	rm->healthPickupTexture = LoadTexture("resources/images/health_pickup.png");
	rm->xpGemTexture = LoadTexture("resources/images/xp_gem.png");

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

	UnloadTexture(rm->grassTexture);
	UnloadTexture(rm->grassVarTexture);
	UnloadTexture(rm->bushTexture);
	UnloadTexture(rm->dirtTexture);
	UnloadTexture(rm->rockTexture);
	UnloadTexture(rm->healthPickupTexture);
	UnloadTexture(rm->xpGemTexture);

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
