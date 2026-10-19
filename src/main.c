#include "Window.h"

int main(void) {
    Window *window = createWindow();
    runWindow(window);
    destroyWindow(window);
    return 0;
}
