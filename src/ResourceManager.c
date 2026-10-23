#include <stdlib.h>

#include "ResourceManager.h"

ResourceManager* createResourceManager(void) {
	ResourceManager* rm = malloc(sizeof(ResourceManager));
	if (rm == NULL) exit(EXIT_FAILURE);
	rm->loaded = 0;
	return rm;
}

void loadResources(ResourceManager* rm) {
	rm->playerTexture = LoadTexture("resources/images/player.png");
	rm->grassTexture = LoadTexture("resources/images/grass.png");
	rm->batTexture = LoadTexture("resources/images/bat.png");
	rm->beetleTexture = LoadTexture("resources/images/beetle.png");
	rm->slimeTexture = LoadTexture("resources/images/slime.png");
	rm->healthPickupTexture = LoadTexture("resources/images/health_pickup.png");
	rm->xpGemTexture = LoadTexture("resources/images/xp_gem.png");
	// rm->jumpSound = LoadSound("resources/sfx/jump.wav");
	// rm->backgroundMusic = LoadMusicStream("resources/musics/theme.ogg");
	rm->loaded = 1;
}

void unloadResources(ResourceManager* rm) {
	if (!rm->loaded) return;

	UnloadTexture(rm->playerTexture);
	UnloadTexture(rm->grassTexture);
	UnloadTexture(rm->batTexture);
	UnloadTexture(rm->beetleTexture);
	UnloadTexture(rm->slimeTexture);
	UnloadTexture(rm->healthPickupTexture);
	UnloadTexture(rm->xpGemTexture);
	// UnloadSound(rm->jumpSound);
	// UnloadMusicStream(rm->backgroundMusic);

	rm->loaded = 0;
}

void destroyResourceManager(ResourceManager* rm) {
	unloadResources(rm);
	free(rm);
}
