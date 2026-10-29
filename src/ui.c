#include "ui.h"
#include "raylib.h"

void drawPlayerHealth(const Player* player) {
	int barWidth = 200;
	int barHeight = 20;

	int marginX = GetScreenWidth() * 0.02f;
	int marginY = GetScreenHeight() * 0.03f;

	float healthPercent = (float) player->health / player->maxHealth;

	int currentWidth = (int) (barWidth * healthPercent);

	DrawRectangle(marginX, marginY, barWidth, barHeight, DARKGRAY);

	DrawRectangle(marginX, marginY, currentWidth, barHeight, GREEN);

	DrawRectangleLines(marginX, marginY, barWidth, barHeight, WHITE);

	DrawText(
	    TextFormat("%d/%d", player->health, player->maxHealth), marginX + 20,
	    marginY + 20, 20, WHITE);
}

void drawTimer(float timer) {
	int marginX = GetScreenWidth() / 2 - 50;
	int marginY = GetScreenHeight() * 0.03f;

	int minutes = (int) timer / 60;
	int seconds = (int) timer % 60;

	DrawText(
	    TextFormat("%02d:%02d", minutes, seconds), marginX, marginY, 50, WHITE);
}

void drawScore(const Player* player) {
	const char* text = TextFormat("Pontos: %d", player->score);
	int fontSize = 30;

	int margin = GetScreenWidth() * 0.02f;
	int x = GetScreenWidth() - MeasureText(text, fontSize) - margin;
	int y = GetScreenHeight() * 0.03f;

	DrawText(text, x, y, fontSize, WHITE);
}

void drawPlayerLevel(const Player* player) {
	int barWidth = 200;
	int barHeight = 10;

	int marginX = GetScreenWidth() * 0.02f;
	int barY = GetScreenHeight() * 0.03f + 45;

	int currentWidth = (int) (barWidth * getPlayerXpProgress(player));

	DrawRectangle(marginX, barY, barWidth, barHeight, DARKGRAY);
	DrawRectangle(marginX, barY, currentWidth, barHeight, SKYBLUE);
	DrawRectangleLines(marginX, barY, barWidth, barHeight, WHITE);

	DrawText(
	    TextFormat("Nivel %d", player->level), marginX, barY + 15, 20, WHITE);

	if (player->levelUpTimer > 0.0f) {
		const char* text = "LEVEL UP!";
		int fontSize = 40;

		DrawText(
		    text, (GetScreenWidth() - MeasureText(text, fontSize)) / 2,
		    GetScreenHeight() / 4, fontSize, YELLOW);
	}
}