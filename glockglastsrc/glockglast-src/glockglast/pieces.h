#ifndef PIECES_H
#define PIECES_H

#include <stdbool.h>
#include "raylib.h"

// Largest bounding box (rows or cols) any piece shape can have.
#define PIECE_MAX_SIZE 5

// A single game piece: a small grid of filled/empty cells plus a color.
typedef struct {
    bool cells[PIECE_MAX_SIZE][PIECE_MAX_SIZE];
    int rows;
    int cols;
    int colorIndex; // 1-based index into the piece color palette
} Piece;

// Picks a random shape from the shape table and a random color.
Piece Piece_GenerateRandom(void);

// Number of filled cells in the piece (used for scoring).
int Piece_CellCount(const Piece *piece);

// Maps a 1-based color index to an actual raylib Color.
Color Piece_GetColor(int colorIndex);

// Draws the piece with its top-left cell at pixel (x, y).
void Piece_Draw(const Piece *piece, int x, int y, int cellSize, float alpha);

#endif // PIECES_H
