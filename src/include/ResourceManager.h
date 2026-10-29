#pragma once

#include "EnemyType.h"
#include "config.h"
#include "raylib.h"

// Ponto único de carregamento/descarregamento de texturas, sons e músicas.
typedef struct ResourceManager {
	Texture2D playerIdleTextures[PLAYER_MAX_LEVEL];
	Texture2D playerWalkTextures[PLAYER_MAX_LEVEL];
	Texture2D playerAttackTextures[PLAYER_MAX_LEVEL];
	Texture2D playerWalkAttackTextures[PLAYER_MAX_LEVEL];
	Texture2D grassTexture;
	Texture2D bushTexture;
	Texture2D grassVarTexture;
	Texture2D dirtTexture;
	Texture2D rockTexture;
	Texture2D healthPickupTexture;
	Texture2D xpGemTexture;
	Texture2D enemyWalkTextures[ENEMY_TYPE_COUNT][ENEMY_VARIANTS];
	Texture2D enemyAttackTextures[ENEMY_TYPE_COUNT][ENEMY_VARIANTS];
	// Sound jumpSound;
	// Music backgroundMusic;
	int loaded;
} ResourceManager;

ResourceManager* createResourceManager(void);
void loadResources(ResourceManager* rm);
void unloadResources(ResourceManager* rm);
void destroyResourceManager(ResourceManager* rm);
