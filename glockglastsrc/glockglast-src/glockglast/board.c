#include "board.h"

void Board_Init(Board *board)
{
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            board->cells[r][c] = 0;
        }
    }
}

bool Board_CanPlacePiece(const Board *board, const Piece *piece, int row, int col)
{
    for (int r = 0; r < piece->rows; r++) {
        for (int c = 0; c < piece->cols; c++) {
            if (!piece->cells[r][c]) continue;

            int boardRow = row + r;
            int boardCol = col + c;

            if (boardRow < 0 || boardRow >= BOARD_SIZE || boardCol < 0 || boardCol >= BOARD_SIZE) {
                return false;
            }
            if (board->cells[boardRow][boardCol] != 0) {
                return false;
            }
        }
    }

    return true;
}

void Board_PlacePiece(Board *board, const Piece *piece, int row, int col)
{
    for (int r = 0; r < piece->rows; r++) {
        for (int c = 0; c < piece->cols; c++) {
            if (!piece->cells[r][c]) continue;
            board->cells[row + r][col + c] = piece->colorIndex;
        }
    }
}

int Board_ClearFullLines(Board *board)
{
    bool fullRows[BOARD_SIZE] = { false };
    bool fullCols[BOARD_SIZE] = { false };

    for (int r = 0; r < BOARD_SIZE; r++) {
        bool full = true;
        for (int c = 0; c < BOARD_SIZE; c++) {
            if (board->cells[r][c] == 0) { full = false; break; }
        }
        fullRows[r] = full;
    }

    for (int c = 0; c < BOARD_SIZE; c++) {
        bool full = true;
        for (int r = 0; r < BOARD_SIZE; r++) {
            if (board->cells[r][c] == 0) { full = false; break; }
        }
        fullCols[c] = full;
    }

    int cleared = 0;

    for (int r = 0; r < BOARD_SIZE; r++) {
        if (!fullRows[r]) continue;
        for (int c = 0; c < BOARD_SIZE; c++) board->cells[r][c] = 0;
        cleared++;
    }

    for (int c = 0; c < BOARD_SIZE; c++) {
        if (!fullCols[c]) continue;
        for (int r = 0; r < BOARD_SIZE; r++) board->cells[r][c] = 0;
        cleared++;
    }

    return cleared;
}

bool Board_HasValidPlacement(const Board *board, const Piece *piece)
{
    for (int r = 0; r <= BOARD_SIZE - piece->rows; r++) {
        for (int c = 0; c <= BOARD_SIZE - piece->cols; c++) {
            if (Board_CanPlacePiece(board, piece, r, c)) return true;
        }
    }

    return false;
}

void Board_Draw(const Board *board, int originX, int originY, int cellSize)
{
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            Rectangle rect = {
                (float)(originX + c * cellSize),
                (float)(originY + r * cellSize),
                (float)(cellSize - 2),
                (float)(cellSize - 2),
            };

            int value = board->cells[r][c];
            if (value == 0) {
                DrawRectangleRounded(rect, 0.2f, 4, (Color){ 45, 49, 63, 255 });
            } else {
                DrawRectangleRounded(rect, 0.25f, 4, Piece_GetColor(value));
            }
        }
    }

    Rectangle border = {
        (float)originX - 4,
        (float)originY - 4,
        (float)(BOARD_SIZE * cellSize) + 4,
        (float)(BOARD_SIZE * cellSize) + 4,
    };
    DrawRectangleRoundedLines(border, 0.05f, 4, (Color){ 90, 96, 120, 255 });
}
