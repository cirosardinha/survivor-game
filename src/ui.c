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