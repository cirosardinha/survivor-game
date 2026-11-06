#include "ui.h"
#include "config.h"

static void drawTextInFrame(
    const ResourceManager* rm, const char* text, Vector2 frameOrigin,
    int frameHeight, float areaX, float areaWidth, float scale);

void drawPlayerHealth(const Player* player, const ResourceManager* rm) {
	float x = GetScreenWidth() * 0.02f;
	float y = GetScreenHeight() * 0.03f;

	float percent = (float) player->health / player->maxHealth;
	if (percent < 0.0f) percent = 0.0f;

	DrawTextureEx(rm->hpFrameTexture, (Vector2){x, y}, 0.0f, UI_SCALE, WHITE);

	int fillWidth = (int) (rm->hpFillTexture.width * percent);
	Rectangle src = {
	    0.0f, 0.0f, (float) fillWidth, (float) rm->hpFillTexture.height};
	Rectangle dest = {
	    x + 25 * UI_SCALE,
	    y + 9 * UI_SCALE,
	    fillWidth * UI_SCALE,
	    rm->hpFillTexture.height * UI_SCALE,
	};
	DrawTexturePro(
	    rm->hpFillTexture, src, dest, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);

	drawTextInFrame(
	    rm, TextFormat("%d/%d", player->health, player->maxHealth),
	    (Vector2){x, y}, rm->hpFrameTexture.height, 25, rm->hpFillTexture.width,
	    UI_SCALE);
}

static void drawTextInFrame(
    const ResourceManager* rm, const char* text, Vector2 frameOrigin,
    int frameHeight, float areaX, float areaWidth, float scale) {
	float fontSize = 15 * scale;
	float spacing = scale;
	Vector2 size = MeasureTextEx(rm->uiFont, text, fontSize, spacing);

	Vector2 pos = {
	    frameOrigin.x + (areaX + areaWidth / 2.0f) * scale - size.x / 2.0f,
	    frameOrigin.y + frameHeight * scale / 2.0f - size.y / 2.0f,
	};
	pos.x = (float) (int) pos.x;
	pos.y = (float) (int) pos.y;

	DrawTextEx(rm->uiFont, text, pos, fontSize, spacing, WHITE);
}

void drawTimer(float timer, const ResourceManager* rm) {
	Texture2D frame = rm->timerFrameTexture;

	Vector2 origin = {
	    (GetScreenWidth() - frame.width * PLATE_SCALE) / 2.0f,
	    GetScreenHeight() * 0.03f,
	};
	DrawTextureEx(frame, origin, 0.0f, PLATE_SCALE, WHITE);

	int minutes = (int) timer / 60;
	int seconds = (int) timer % 60;
	drawTextInFrame(
	    rm, TextFormat("%02d:%02d", minutes, seconds), origin, frame.height, 33,
	    53, PLATE_SCALE);
}

void drawScore(const Player* player, const ResourceManager* rm) {
	Texture2D frame = rm->scoreFrameTexture;

	float margin = GetScreenWidth() * 0.02f;
	Vector2 origin = {
	    GetScreenWidth() - frame.width * PLATE_SCALE - margin,
	    GetScreenHeight() * 0.03f,
	};
	DrawTextureEx(frame, origin, 0.0f, PLATE_SCALE, WHITE);

	drawTextInFrame(
	    rm, TextFormat("%d", player->score), origin, frame.height, 31, 72,
	    PLATE_SCALE);
}

void drawPlayerLevel(const Player* player, const ResourceManager* rm) {
	float x = GetScreenWidth() * 0.02f;

	float y = GetScreenHeight() * 0.03f + (25 + 1) * UI_SCALE;

	DrawTextureEx(rm->xpFrameTexture, (Vector2){x, y}, 0.0f, UI_SCALE, WHITE);

	int fillWidth =
	    (int) (rm->xpFillTexture.width * getPlayerXpProgress(player));
	Rectangle src = {
	    0.0f, 0.0f, (float) fillWidth, (float) rm->xpFillTexture.height};
	Rectangle dest = {
	    x + 25 * UI_SCALE,
	    y + 8 * UI_SCALE,
	    fillWidth * UI_SCALE,
	    rm->xpFillTexture.height * UI_SCALE,
	};
	DrawTexturePro(
	    rm->xpFillTexture, src, dest, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);

	DrawTextEx(
	    rm->uiFont, TextFormat("Nível %d", player->level),
	    (Vector2){x, y + 24 * UI_SCALE + 6}, 30, 2, WHITE);
}
void drawButton(Font font, Rectangle button, const char* label) {
	bool hover = CheckCollisionPointRec(GetMousePosition(), button);
	Color fill = hover ? (Color){62, 96, 80, 255} : (Color){40, 62, 53, 255};

	DrawRectangleRec(button, fill);
	DrawRectangleLinesEx(button, 3, (Color){108, 152, 118, 255});

	float size = 30;
	float spacing = 2;
	Vector2 textSize = MeasureTextEx(font, label, size, spacing);

	Vector2 pos = {
	    (float) (int) (button.x + (button.width - textSize.x) / 2.0f),
	    (float) (int) (button.y + (button.height - textSize.y) / 2.0f),
	};
	DrawTextEx(font, label, pos, size, spacing, (Color){236, 236, 235, 255});
}

void drawCenteredLine(
    Font font, const char* text, int y, int size, Color color) {
	float spacing = (float) (size / 15);
	Vector2 textSize = MeasureTextEx(font, text, (float) size, spacing);

	Vector2 pos = {
	    (float) (int) ((WINDOW_WIDTH - textSize.x) / 2.0f), (float) y};
	DrawTextEx(font, text, pos, (float) size, spacing, color);
}