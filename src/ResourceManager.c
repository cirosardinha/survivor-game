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
	rm->playerIdleTexture =
	    LoadTexture("resources/images/player/swordsman1_idle.png");
	rm->playerWalkTexture =
	    LoadTexture("resources/images/player/swordsman1_walk.png");
	rm->playerAttackTexture =
	    LoadTexture("resources/images/player/swordsman1_attack.png");
	rm->playerWalkAttackTexture =
	    LoadTexture("resources/images/player/swordsman1_walk_attack.png");
	rm->grassTexture = LoadTexture("resources/images/grass.png");
	rm->grassVarTexture = LoadTexture("resources/images/grass_var.png");
	rm->bushTexture = LoadTexture("resources/images/bush.png");
	rm->rockTexture = LoadTexture("resources/images/rock.png");
	rm->healthPickupTexture = LoadTexture("resources/images/health_pickup.png");
	rm->xpGemTexture = LoadTexture("resources/images/xp_gem.png");

	char path[96];
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
	UnloadTexture(rm->playerIdleTexture);
	UnloadTexture(rm->playerWalkTexture);
	UnloadTexture(rm->playerAttackTexture);
	UnloadTexture(rm->playerWalkAttackTexture);
	UnloadTexture(rm->grassTexture);
	UnloadTexture(rm->grassVarTexture);
	UnloadTexture(rm->bushTexture);
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
