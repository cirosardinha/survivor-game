#pragma once

#include <stdbool.h>

#define RANKING_SIZE 3

void loadRanking(void);
bool addRankingScore(int score);
int getRankingScore(int index);