#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include "raylib.h"
#include "board.h"
#include "pieces.h"

#define TRAY_SIZE 3

// Layout: shared by game.c (to place things) and main.c (to size the window).
#define CELL_SIZE 46
#define TRAY_CELL_SIZE 30
#define BOARD_MARGIN 24
#define BOARD_ORIGIN_X BOARD_MARGIN
#define BOARD_ORIGIN_Y 110
#define TRAY_ORIGIN_Y (BOARD_ORIGIN_Y + BOARD_SIZE * CELL_SIZE + 40)
#define TRAY_HEIGHT 150

#define SCREEN_WIDTH (BOARD_MARGIN * 2 + BOARD_SIZE * CELL_SIZE)
#define SCREEN_HEIGHT (TRAY_ORIGIN_Y + TRAY_HEIGHT)

typedef struct {
    Board board;

    Piece tray[TRAY_SIZE];
    bool traySlotFilled[TRAY_SIZE];

    int score;
    int highScore;
    bool isNewHighScore;
    bool gameOver;

    int draggingSlot;       // -1 when nothing is being dragged
    Vector2 dragMousePos;
    int dragRow, dragCol;   // board cell the dragged piece is hovering over
    bool dragValid;
} Game;

void Game_Init(Game *game);
void Game_Update(Game *game);
void Game_Draw(Game *game);

#endif // GAME_H
