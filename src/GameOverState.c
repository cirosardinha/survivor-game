#include "Game.h"
#include "States.h"
#include "config.h"
#include "ui.h"

static void drawRankingList(Font font, int y);
static Rectangle gameOverMenuButton(void);
static Rectangle gameOverResetButton(void);

void enterGameOver(Game* game) {
	game->newRecord = addRankingScore(game->player.score);
	changeGameState(game, STATE_GAMEOVER);
}

void updateGameOver(Game* game) {
	Vector2 mouse = GetMousePosition();

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		if (CheckCollisionPointRec(mouse, gameOverMenuButton())) {
			changeGameState(game, STATE_MENU);
		} else if (CheckCollisionPointRec(mouse, gameOverResetButton())) {
			resetGame(game);
			changeGameState(game, STATE_GAMEPLAY);
		}
	}
}

void drawGameOver(Game* game) {
	Font font = game->resourceManager->uiFont;

	DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, (Color){0, 0, 0, 180});

	drawCenteredLine(font, "GAME OVER", 50, 60, RED);

	int minutes = (int) game->gameTimer / 60;
	int seconds = (int) game->gameTimer % 60;
	drawCenteredLine(
	    font,
	    TextFormat(
	        "Pontos: %d    Tempo: %02d:%02d", game->player.score, minutes,
	        seconds),
	    125, 30, WHITE);

	if (game->newRecord) {
		drawCenteredLine(font, "Novo recorde!", 168, 30, YELLOW);
	}

	drawRankingList(font, 215);

	drawButton(font, gameOverMenuButton(), "Voltar ao menu");
	drawButton(font, gameOverResetButton(), "Reset");
}

static void drawRankingList(Font font, int y) {
	drawCenteredLine(font, "Melhores pontuacoes", y, 30, YELLOW);

	for (int i = 0; i < RANKING_SIZE; i++) {
		int score = getRankingScore(i);

		const char* text = score > 0 ? TextFormat("%d. %d", i + 1, score)
		                             : TextFormat("%d. -", i + 1);

		drawCenteredLine(font, text, y + 40 + i * 32, 30, WHITE);
	}
}

static Rectangle gameOverMenuButton(void) {
	return (Rectangle){WINDOW_WIDTH / 2.0f - 150, 430, 300, 50};
}

static Rectangle gameOverResetButton(void) {
	return (Rectangle){WINDOW_WIDTH / 2.0f - 150, 495, 300, 50};
}