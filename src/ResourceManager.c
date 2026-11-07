#include <stdio.h>
#include <stdlib.h>

#include "ResourceManager.h"

#define SFX_VOLUME 0.5f

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
	rm->orbUpgradeTexture = LoadTexture("resources/images/upgrades/orb.png");

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
	}

	rm->startSound = LoadSound("resources/sfx/start.wav");
	rm->clickSound = LoadSound("resources/sfx/ui_click.wav");
	rm->playerAttackSound = LoadSound("resources/sfx/player_attack.wav");
	rm->healthPickupSound = LoadSound("resources/sfx/health_pickup.wav");
	rm->xpPickupSound = LoadSound("resources/sfx/xp_pickup.wav");
	rm->levelUpSound = LoadSound("resources/sfx/level_up.wav");
	rm->playerHurtSound = LoadSound("resources/sfx/player_hurt.wav");
	rm->enemyHurtSound = LoadSound("resources/sfx/enemy_hurt.wav");
	rm->enemyDeathSound = LoadSound("resources/sfx/enemy_death.wav");
	rm->orcAttackSound = LoadSound("resources/sfx/orc_attack.wav");
	rm->plantAttackSound = LoadSound("resources/sfx/plant_attack.wav");
	rm->slimeAttackSound = LoadSound("resources/sfx/slime_attack.wav");
	rm->vampireAttackSound = LoadSound("resources/sfx/vampire_attack.wav");
	rm->gameOverSound = LoadSound("resources/sfx/game_over.wav");

	rm->backgroundMusic = LoadMusicStream("resources/musics/bg_music.ogg");

	SetMusicVolume(rm->backgroundMusic, 0.2f);

	SetSoundVolume(rm->startSound, SFX_VOLUME);
	SetSoundVolume(rm->clickSound, SFX_VOLUME);
	SetSoundVolume(rm->playerAttackSound, SFX_VOLUME);
	SetSoundVolume(rm->playerHurtSound, SFX_VOLUME);
	SetSoundVolume(rm->enemyHurtSound, SFX_VOLUME);
	SetSoundVolume(rm->enemyDeathSound, SFX_VOLUME);
	SetSoundVolume(rm->healthPickupSound, SFX_VOLUME);
	SetSoundVolume(rm->xpPickupSound, SFX_VOLUME);
	SetSoundVolume(rm->levelUpSound, SFX_VOLUME);
	SetSoundVolume(rm->orcAttackSound, SFX_VOLUME);
	SetSoundVolume(rm->plantAttackSound, SFX_VOLUME);
	SetSoundVolume(rm->slimeAttackSound, SFX_VOLUME);
	SetSoundVolume(rm->vampireAttackSound, SFX_VOLUME);
	SetSoundVolume(rm->gameOverSound, SFX_VOLUME);

	rm->loaded = 1;
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
	UnloadTexture(rm->orbUpgradeTexture);

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
	UnloadMusicStream(rm->backgroundMusic);

	UnloadSound(rm->startSound);
	UnloadSound(rm->clickSound);
	UnloadSound(rm->playerAttackSound);
	UnloadSound(rm->playerHurtSound);
	UnloadSound(rm->enemyHurtSound);
	UnloadSound(rm->enemyDeathSound);
	UnloadSound(rm->healthPickupSound);
	UnloadSound(rm->xpPickupSound);
	UnloadSound(rm->levelUpSound);
	UnloadSound(rm->orcAttackSound);
	UnloadSound(rm->vampireAttackSound);
	UnloadSound(rm->plantAttackSound);
	UnloadSound(rm->slimeAttackSound);
	UnloadSound(rm->gameOverSound);

	rm->loaded = 0;
}

void destroyResourceManager(ResourceManager* rm) {
	unloadResources(rm);
	free(rm);
}
