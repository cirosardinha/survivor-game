#include <stdlib.h>

#include "raylib.h"
#include "config.h"
#include "Window.h"

Window *createWindow(void) {
    Window *window = malloc(sizeof(Window));
    if (window == NULL) exit(EXIT_FAILURE);
    window->game = NULL;
    return window;
}

void runWindow(Window *window) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    // InitAudioDevice();
    SetTargetFPS(TARGET_FPS);
    SetExitKey(KEY_NULL);

    window->game = createGame();

    while (!WindowShouldClose()) {
        updateGame(window->game);

        BeginDrawing();
            ClearBackground(RAYWHITE);
            drawGame(window->game);
        EndDrawing();
    }

    destroyGame(window->game);
    // CloseAudioDevice();
    CloseWindow();
}

void destroyWindow(Window *window) {
    free(window);
}
