#pragma once

#include "EnemyType.h"
#include "config.h"
#include "raylib.h"

// Ponto único de carregamento/descarregamento de texturas, sons e músicas.
typedef struct ResourceManager {
	Font uiFont;
	Texture2D hpFrameTexture;
	Texture2D hpFillTexture;
	Texture2D xpFrameTexture;
	Texture2D xpFillTexture;
	Texture2D scoreFrameTexture;
	Texture2D timerFrameTexture;
	Texture2D enemyHpBackTexture;
	Texture2D enemyHpFillTexture;

	Texture2D playerIdleTextures[PLAYER_SPRITE_TIERS];
	Texture2D playerWalkTextures[PLAYER_SPRITE_TIERS];
	Texture2D playerAttackTextures[PLAYER_SPRITE_TIERS];
	Texture2D playerWalkAttackTextures[PLAYER_SPRITE_TIERS];
	Texture2D groundTilesetTexture;
	Texture2D grassVarTexture;
	Texture2D rockTexture;
	Texture2D healthPickupTexture;
	Texture2D xpGemTexture;
	Texture2D enemyWalkTextures[ENEMY_TYPE_COUNT][ENEMY_VARIANTS];
	Texture2D enemyAttackTextures[ENEMY_TYPE_COUNT][ENEMY_VARIANTS];

	Music backgroundMusic;

	Sound startSound;
	Sound clickSound;
	Sound playerAttackSound;
	Sound playerHurtSound;
	Sound enemyHurtSound;
	Sound enemyDeathSound;
	Sound orcAttackSound;
	Sound vampireAttackSound;
	Sound plantAttackSound;
	Sound slimeAttackSound;
	Sound healthPickupSound;
	Sound xpPickupSound;
	Sound levelUpSound;
	Sound gameOverSound;
	int loaded;
} ResourceManager;

ResourceManager* createResourceManager(void);
void loadResources(ResourceManager* rm);
void unloadResources(ResourceManager* rm);
void destroyResourceManager(ResourceManager* rm);
