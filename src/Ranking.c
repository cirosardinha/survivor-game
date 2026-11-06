#include "Ranking.h"

#include "raylib.h"
#include <string.h>

#define RANKING_FILE "highscores.dat"

static int scores[RANKING_SIZE];

static void saveRanking(void);

void loadRanking(void) {
	memset(scores, 0, sizeof(scores));

	if (!FileExists(RANKING_FILE)) {
		return;
	}

	int size = 0;
	unsigned char* data = LoadFileData(RANKING_FILE, &size);

	if (data == NULL) return;

	if (size == (int) sizeof(scores)) {
		memcpy(scores, data, sizeof(scores));
	}

	UnloadFileData(data);
}

bool addRankingScore(int score) {
	if (score <= 0 || score <= scores[RANKING_SIZE - 1]) return false;

	int position = 0;
	while (scores[position] >= score) {
		position++;
	}

	for (int i = RANKING_SIZE - 1; i > position; i--) {
		scores[i] = scores[i - 1];
	}

	scores[position] = score;
	saveRanking();

	return true;
}

int getRankingScore(int index) {
	return scores[index];
}

static void saveRanking(void) {
	SaveFileData(RANKING_FILE, scores, sizeof(scores));
}