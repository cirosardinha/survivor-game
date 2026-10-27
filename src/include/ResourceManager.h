#pragma once

#include "EnemyType.h"
#include "raylib.h"

// Ponto único de carregamento/descarregamento de texturas, sons e músicas.
typedef struct ResourceManager {
	Texture2D playerIdleTexture;
	Texture2D playerWalkTexture;
	Texture2D playerAttackTexture;
	Texture2D playerWalkAttackTexture;
	Texture2D grassTexture;
	Texture2D bushTexture;
	Texture2D grassVarTexture;
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
