#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>
#include "raylib.h"
#include "pieces.h"

#define BOARD_SIZE 8

// 0 means empty, any other value is a 1-based piece color index.
typedef struct {
    int cells[BOARD_SIZE][BOARD_SIZE];
} Board;

void Board_Init(Board *board);

// True if every filled cell of the piece would land on the board and on
// currently empty cells, when its top-left corner sits at (row, col).
bool Board_CanPlacePiece(const Board *board, const Piece *piece, int row, int col);

// Stamps the piece's filled cells onto the board. Caller must have
// verified Board_CanPlacePiece() first.
void Board_PlacePiece(Board *board, const Piece *piece, int row, int col);

// Clears every fully-filled row and column, returns how many lines were
// cleared (rows + columns).
int Board_ClearFullLines(Board *board);

// True if there is at least one (row, col) where the piece could be placed.
bool Board_HasValidPlacement(const Board *board, const Piece *piece);

void Board_Draw(const Board *board, int originX, int originY, int cellSize);

#endif // BOARD_H
