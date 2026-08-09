#include "raylib.h"
#include "game.h"

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Glock Glast");
    SetTargetFPS(60);

    Game game;
    Game_Init(&game);

    while (!WindowShouldClose()) {
        Game_Update(&game);

        BeginDrawing();
        ClearBackground((Color){ 24, 26, 34, 255 });
        Game_Draw(&game);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
