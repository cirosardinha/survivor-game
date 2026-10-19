#pragma once

#include "raylib.h"

// Ponto único de carregamento/descarregamento de texturas, sons e músicas.
typedef struct ResourceManager {
    // Texture2D playerTexture;
    // Sound jumpSound;
    // Music backgroundMusic;
    int loaded;
} ResourceManager;

ResourceManager *createResourceManager(void);
void loadResources(ResourceManager *rm);
void unloadResources(ResourceManager *rm);
void destroyResourceManager(ResourceManager *rm);
