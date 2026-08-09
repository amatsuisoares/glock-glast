#include "pieces.h"
#include <string.h>

// Each template is drawn with '\n' separating rows and 'X' marking a
// filled cell. Anything else (typically '.') is treated as empty.
static const char *SHAPE_TEMPLATES[] = {
    // Single block
    "X",

    // Dominoes
    "XX",
    "X\nX",

    // Trominoes (line)
    "XXX",
    "X\nX\nX",

    // Trominoes (corner)
    "X.\nXX",
    "XX\nX.",
    "XX\n.X",
    ".X\nXX",

    // Tetrominoes (line)
    "XXXX",
    "X\nX\nX\nX",

    // Tetromino square
    "XX\nXX",

    // Tetromino L / J (all 4 rotations each)
    "X.\nX.\nXX",
    "XXX\nX..",
    "XX\n.X\n.X",
    "..X\nXXX",

    ".X\n.X\nXX",
    "X..\nXXX",
    "XX\nX.\nX.",
    "XXX\n..X",

    // Tetromino T (all 4 rotations)
    "XXX\n.X.",
    ".X\nXX\n.X",
    ".X.\nXXX",
    "X.\nXX\nX.",

    // Tetromino S / Z
    ".XX\nXX.",
    "X.\nXX\n.X",
    "XX.\n.XX",
    ".X\nXX\nX.",

    // Pentominoes (line)
    "XXXXX",
    "X\nX\nX\nX\nX",

    // Big square (3x3 block)
    "XXX\nXXX\nXXX",

    // Big corners (3x3 L, all 4 rotations)
    "X..\nX..\nXXX",
    "XXX\nX..\nX..",
    "XXX\n..X\n..X",
    "..X\n..X\nXXX",

    // Plus / cross pentomino
    ".X.\nXXX\n.X.",
};

#define SHAPE_COUNT (int)(sizeof(SHAPE_TEMPLATES) / sizeof(SHAPE_TEMPLATES[0]))

static const Color PIECE_COLORS[] = {
    {235, 87, 87, 255},   // red
    {242, 153, 74, 255},  // orange
    {242, 201, 76, 255},  // yellow
    {111, 207, 151, 255}, // green
    {86, 174, 235, 255},  // blue
    {155, 129, 235, 255}, // purple
    {235, 129, 194, 255}, // pink
};

#define PIECE_COLOR_COUNT (int)(sizeof(PIECE_COLORS) / sizeof(PIECE_COLORS[0]))

static void ParseShape(const char *tpl, Piece *piece)
{
    memset(piece->cells, 0, sizeof(piece->cells));

    int row = 0;
    int col = 0;
    int maxCol = 0;

    for (const char *ch = tpl; *ch != '\0'; ch++) {
        if (*ch == '\n') {
            row++;
            col = 0;
            continue;
        }

        if (*ch == 'X') {
            piece->cells[row][col] = true;
        }
        col++;
        if (col > maxCol) maxCol = col;
    }

    piece->rows = row + 1;
    piece->cols = maxCol;
}

Piece Piece_GenerateRandom(void)
{
    Piece piece = { 0 };

    int shapeIndex = GetRandomValue(0, SHAPE_COUNT - 1);
    ParseShape(SHAPE_TEMPLATES[shapeIndex], &piece);

    piece.colorIndex = GetRandomValue(1, PIECE_COLOR_COUNT);

    return piece;
}

int Piece_CellCount(const Piece *piece)
{
    int count = 0;

    for (int r = 0; r < piece->rows; r++) {
        for (int c = 0; c < piece->cols; c++) {
            if (piece->cells[r][c]) count++;
        }
    }

    return count;
}

Color Piece_GetColor(int colorIndex)
{
    if (colorIndex < 1 || colorIndex > PIECE_COLOR_COUNT) return GRAY;
    return PIECE_COLORS[colorIndex - 1];
}

void Piece_Draw(const Piece *piece, int x, int y, int cellSize, float alpha)
{
    Color fill = Fade(Piece_GetColor(piece->colorIndex), alpha);
    Color highlight = Fade(WHITE, alpha * 0.35f);

    for (int r = 0; r < piece->rows; r++) {
        for (int c = 0; c < piece->cols; c++) {
            if (!piece->cells[r][c]) continue;

            Rectangle rect = {
                (float)(x + c * cellSize),
                (float)(y + r * cellSize),
                (float)(cellSize - 2),
                (float)(cellSize - 2),
            };

            DrawRectangleRounded(rect, 0.25f, 4, fill);

            Rectangle inner = { rect.x + 3, rect.y + 3, rect.width - 6, rect.height * 0.35f };
            if (inner.width > 0 && inner.height > 0) {
                DrawRectangleRounded(inner, 0.4f, 4, highlight);
            }
        }
    }
}
