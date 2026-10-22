#pragma once

#include "Enemy.h"
#include "Player.h"

void updatePlayerAttack(Player* player, Enemy* enemies, int count, float delta);
void updateEnemyAttack(Enemy* enemy, Player* player, float delta);