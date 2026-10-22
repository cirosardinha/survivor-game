#pragma once

#include "Enemy.h"
#include "Player.h"

void resolveEnemyPlayerCollision(Enemy* enemy, Player* player, float delta);
void resolveAllEnemiesCollision(Enemy* enemies, int enemyCount, float delta);