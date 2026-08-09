#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// How far above the mouse cursor a picked-up piece floats, so the player
// can still see the board cells underneath it while dragging.
static const int DRAG_LIFT = 40;

#define HIGH_SCORE_FILE "highscore.txt"

// Stores the high score under the user's profile (e.g. %APPDATA%\glockglast)
// instead of next to the .exe, so it's shared no matter which build/copy of
// the game is launched.
static void BuildHighScorePath(char *buffer, size_t size)
{
#if defined(_WIN32)
    const char *appData = getenv("APPDATA");
    if (appData != NULL && appData[0] != '\0') {
        char dir[440];
        snprintf(dir, sizeof(dir), "%s/glockglast", appData);
        if (!DirectoryExists(dir)) MakeDirectory(dir);
        snprintf(buffer, size, "%s/%s", dir, HIGH_SCORE_FILE);
        return;
    }
#endif
    const char *appDir = GetApplicationDirectory();
    size_t appDirLength = strlen(appDir);
    const char *separator = (appDirLength > 0 && (appDir[appDirLength - 1] == '/' || appDir[appDirLength - 1] == '\\')) ? "" : "/";
    snprintf(buffer, size, "%s%s%s", appDir, separator, HIGH_SCORE_FILE);
}

static int LoadHighScore(void)
{
    char highScorePath[512];
    BuildHighScorePath(highScorePath, sizeof(highScorePath));

    FILE *file = fopen(highScorePath, "r");
    if (!file) return 0;

    int highScore = 0;
    if (fscanf(file, "%d", &highScore) != 1) highScore = 0;

    fclose(file);
    return highScore;
}

static void SaveHighScore(int highScore)
{
    char highScorePath[512];
    BuildHighScorePath(highScorePath, sizeof(highScorePath));

    FILE *file = fopen(highScorePath, "w");
    if (!file) return;

    fprintf(file, "%d\n", highScore);
    fclose(file);
}

static Rectangle GetTraySlotRect(int slot)
{
    float slotWidth = (float)SCREEN_WIDTH / TRAY_SIZE;
    return (Rectangle){ slotWidth * slot, (float)TRAY_ORIGIN_Y, slotWidth, (float)TRAY_HEIGHT };
}

static bool TrayHasValidPlacement(const Board *board, const Piece tray[TRAY_SIZE])
{
    for (int i = 0; i < TRAY_SIZE; i++) {
        if (Board_HasValidPlacement(board, &tray[i])) return true;
    }
    return false;
}

// Keeps re-rolling the whole batch until at least one of the 3 pieces
// actually fits somewhere on the board, so the player never loses right
// after a refill just because the draw was unlucky. Bounded so a truly
// full board (a real game over) still terminates.
#define TRAY_REROLL_ATTEMPTS 50

static void RefillTrayIfEmpty(Game *game)
{
    for (int i = 0; i < TRAY_SIZE; i++) {
        if (game->traySlotFilled[i]) return;
    }

    Piece candidates[TRAY_SIZE];
    int attempt = 0;

    do {
        for (int i = 0; i < TRAY_SIZE; i++) {
            candidates[i] = Piece_GenerateRandom();
        }
        attempt++;
    } while (!TrayHasValidPlacement(&game->board, candidates) && attempt < TRAY_REROLL_ATTEMPTS);

    for (int i = 0; i < TRAY_SIZE; i++) {
        game->tray[i] = candidates[i];
        game->traySlotFilled[i] = true;
    }
}

static void UpdateGameOverState(Game *game)
{
    for (int i = 0; i < TRAY_SIZE; i++) {
        if (!game->traySlotFilled[i]) continue;
        if (Board_HasValidPlacement(&game->board, &game->tray[i])) return;
    }

    game->gameOver = true;
}

void Game_Init(Game *game)
{
    Board_Init(&game->board);

    for (int i = 0; i < TRAY_SIZE; i++) {
        game->tray[i] = Piece_GenerateRandom();
        game->traySlotFilled[i] = true;
    }

    game->score = 0;
    game->highScore = LoadHighScore();
    game->isNewHighScore = false;
    game->gameOver = false;
    game->draggingSlot = -1;
}

static void BeginDrag(Game *game, Vector2 mouse)
{
    for (int i = 0; i < TRAY_SIZE; i++) {
        if (!game->traySlotFilled[i]) continue;
        if (CheckCollisionPointRec(mouse, GetTraySlotRect(i))) {
            game->draggingSlot = i;
            game->dragMousePos = mouse;
            return;
        }
    }
}

static void UpdateDrag(Game *game, Vector2 mouse)
{
    Piece *piece = &game->tray[game->draggingSlot];
    game->dragMousePos = mouse;

    float topLeftX = mouse.x - (piece->cols * CELL_SIZE) / 2.0f;
    float topLeftY = mouse.y - (piece->rows * CELL_SIZE) / 2.0f - DRAG_LIFT;

    game->dragCol = (int)((topLeftX - BOARD_ORIGIN_X) / CELL_SIZE + 0.5f);
    game->dragRow = (int)((topLeftY - BOARD_ORIGIN_Y) / CELL_SIZE + 0.5f);
    game->dragValid = Board_CanPlacePiece(&game->board, piece, game->dragRow, game->dragCol);
}

static void EndDrag(Game *game)
{
    Piece *piece = &game->tray[game->draggingSlot];

    if (game->dragValid) {
        Board_PlacePiece(&game->board, piece, game->dragRow, game->dragCol);

        int cellsPlaced = Piece_CellCount(piece);
        int linesCleared = Board_ClearFullLines(&game->board);

        game->score += cellsPlaced + linesCleared * linesCleared * 10;
        game->traySlotFilled[game->draggingSlot] = false;

        if (game->score > game->highScore) {
            game->highScore = game->score;
            game->isNewHighScore = true;
            SaveHighScore(game->highScore);
        }

        RefillTrayIfEmpty(game);
        UpdateGameOverState(game);
    }

    game->draggingSlot = -1;
}

void Game_Update(Game *game)
{
    if (game->gameOver) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyPressed(KEY_R)) {
            Game_Init(game);
        }
        return;
    }

    Vector2 mouse = GetMousePosition();

    if (game->draggingSlot == -1) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) BeginDrag(game, mouse);
        return;
    }

    UpdateDrag(game, mouse);

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) EndDrag(game);
}

static void DrawHeader(const Game *game)
{
    DrawText("GLOCK GLAST", BOARD_ORIGIN_X, 24, 30, RAYWHITE);
    DrawText(TextFormat("Score: %d", game->score), BOARD_ORIGIN_X, 64, 22, (Color){ 190, 195, 210, 255 });

    const char *highScoreText = TextFormat("High Score: %d", game->highScore);
    int highScoreWidth = MeasureText(highScoreText, 22);
    DrawText(highScoreText, SCREEN_WIDTH - BOARD_ORIGIN_X - highScoreWidth, 64, 22, (Color){ 190, 195, 210, 255 });
}

static void DrawDragPreview(const Game *game)
{
    if (game->draggingSlot == -1) return;

    const Piece *piece = &game->tray[game->draggingSlot];
    Color color = Fade(game->dragValid ? (Color){ 111, 207, 151, 255 } : (Color){ 235, 87, 87, 255 }, 0.45f);

    for (int r = 0; r < piece->rows; r++) {
        for (int c = 0; c < piece->cols; c++) {
            if (!piece->cells[r][c]) continue;

            int boardRow = game->dragRow + r;
            int boardCol = game->dragCol + c;
            if (boardRow < 0 || boardRow >= BOARD_SIZE || boardCol < 0 || boardCol >= BOARD_SIZE) continue;

            Rectangle rect = {
                (float)(BOARD_ORIGIN_X + boardCol * CELL_SIZE),
                (float)(BOARD_ORIGIN_Y + boardRow * CELL_SIZE),
                (float)(CELL_SIZE - 2),
                (float)(CELL_SIZE - 2),
            };
            DrawRectangleRounded(rect, 0.2f, 4, color);
        }
    }
}

static void DrawTray(const Game *game)
{
    for (int i = 0; i < TRAY_SIZE; i++) {
        Rectangle slot = GetTraySlotRect(i);

        Rectangle slotBg = { slot.x + 6, slot.y + 6, slot.width - 12, slot.height - 12 };
        DrawRectangleRounded(slotBg, 0.08f, 4, (Color){ 34, 37, 48, 255 });

        if (!game->traySlotFilled[i] || i == game->draggingSlot) continue;

        const Piece *piece = &game->tray[i];
        int pieceWidth = piece->cols * TRAY_CELL_SIZE;
        int pieceHeight = piece->rows * TRAY_CELL_SIZE;
        int x = (int)(slot.x + slot.width / 2 - pieceWidth / 2.0f);
        int y = (int)(slot.y + slot.height / 2 - pieceHeight / 2.0f);

        Piece_Draw(piece, x, y, TRAY_CELL_SIZE, 1.0f);
    }
}

static void DrawFloatingPiece(const Game *game)
{
    if (game->draggingSlot == -1) return;

    const Piece *piece = &game->tray[game->draggingSlot];
    int x = (int)(game->dragMousePos.x - (piece->cols * CELL_SIZE) / 2.0f);
    int y = (int)(game->dragMousePos.y - (piece->rows * CELL_SIZE) / 2.0f - DRAG_LIFT);

    Piece_Draw(piece, x, y, CELL_SIZE, 0.9f);
}

static void DrawCenteredText(const char *text, int y, int fontSize, Color color)
{
    int width = MeasureText(text, fontSize);
    DrawText(text, (SCREEN_WIDTH - width) / 2, y, fontSize, color);
}

static void DrawGameOverOverlay(const Game *game)
{
    if (!game->gameOver) return;

    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.7f));

    DrawCenteredText("GAME OVER", SCREEN_HEIGHT / 2 - 60, 40, RAYWHITE);
    DrawCenteredText(TextFormat("Score final: %d", game->score), SCREEN_HEIGHT / 2, 22, (Color){ 190, 195, 210, 255 });

    if (game->isNewHighScore) {
        DrawCenteredText("Novo recorde!", SCREEN_HEIGHT / 2 + 30, 20, (Color){ 240, 200, 90, 255 });
    } else {
        DrawCenteredText(TextFormat("Recorde: %d", game->highScore), SCREEN_HEIGHT / 2 + 30, 20, (Color){ 150, 155, 170, 255 });
    }

    DrawCenteredText("Clique para jogar novamente", SCREEN_HEIGHT / 2 + 62, 18, (Color){ 150, 155, 170, 255 });
}

void Game_Draw(Game *game)
{
    DrawHeader(game);
    Board_Draw(&game->board, BOARD_ORIGIN_X, BOARD_ORIGIN_Y, CELL_SIZE);
    DrawDragPreview(game);
    DrawTray(game);
    DrawFloatingPiece(game);
    DrawGameOverOverlay(game);
}
