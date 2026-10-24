#pragma once

#include "raylib.h"

// Ponto único de carregamento/descarregamento de texturas, sons e músicas.
typedef struct ResourceManager {
	Texture2D playerTexture;
	Texture2D grassTexture;
	Texture2D bushTexture;
	Texture2D grassVarTexture;
	Texture2D rockTexture;
	Texture2D batTexture;
	Texture2D beetleTexture;
	Texture2D slimeTexture;
	Texture2D healthPickupTexture;
	Texture2D xpGemTexture;
	// Sound jumpSound;
	// Music backgroundMusic;
	int loaded;
} ResourceManager;

ResourceManager* createResourceManager(void);
void loadResources(ResourceManager* rm);
void unloadResources(ResourceManager* rm);
void destroyResourceManager(ResourceManager* rm);
