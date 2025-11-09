#include <stddef.h>
#include <raylib.h>
#include <raymath.h>

#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "./nob.h"

#define SW 1200
#define SH 720
#define BLACK_SQR_COLOR 0x1c1916ff
#define CRAPPY_DIM 100

typedef enum {
  None,
  Pawn,
  Rook,
  Knight,
  Bishop,
  Queen,
  King,
} Piece;

typedef struct {
  Rectangle rect;
  bool is_dark;
  size_t row;
  size_t col;
  Piece piece;
} Cell;

typedef struct {
  Cell *items;
  size_t count;
  size_t capacity;
} Cells;

size_t GetPieceValue(Piece p) {
  switch (p) {
    case Pawn: return 1;
    case Knight:
    case Bishop: return 3;
    case Rook: return 5;
    case Queen: return 9;
    default: return 0;
  }
}

Rectangle CreateBoard() {
    size_t dim = GetScreenHeight() * 0.9;
    size_t x = GetScreenWidth() / 2 - dim / 2;
    size_t y = GetScreenHeight() / 2 - dim / 2;
    Rectangle r = { .x = x, .y = y, .width = dim, .height = dim};
    return r;
}

void CreateCells(Rectangle board, Cells* cells) {
    size_t cell_dim = board.width / 8;
    size_t y_start = board.y;
    size_t x_start = board.x;
    for (size_t row = 0; row < 8; ++row) {
        size_t y = y_start + cell_dim * row;
        for (size_t col = 0; col < 8; ++col) {
            size_t x = x_start + cell_dim * col;
            Rectangle r = { .x = x, .y = y, .width = cell_dim, .height = cell_dim };
            Cell* c = malloc(sizeof(Cell));
            c->rect = r;
            c->is_dark = ((row % 2 == 0 && col % 2 == 1) || (row % 2 == 1 && col % 2 == 0));
            c->row = row;
            c->col = col;
            c->piece = None;
            if (row == 0 || row == 7) {
              if (col == 0 || col == 7) {
                c->piece = Rook;
              } else if (col == 1 || col == 6) {
                c->piece = Knight;
              } else if (col == 2 || col == 5) {
                c->piece = Bishop;
              } else if (col == 3) {
                c->piece = row == 0 ? Queen : King;
              } else if (col == 4) {
                c->piece = row == 0 ? King : Queen;
              }
            } else if (row == 1 || row == 6) {
              c->piece = Pawn;
            }
            da_append(cells, *c);
        }
    }
}

int main(void)
{
    InitWindow(SW, SH, "Chess");

    const char* crappy_png = "./assets/crappy/crappy.png";
    Texture2D crappy = LoadTexture(crappy_png);

    Cells cells = {0};
    Rectangle board = CreateBoard();
    CreateCells(board, &cells);
    Color black = GetColor(BLACK_SQR_COLOR);

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(GetColor(0xb77e38FF));

        DrawRectangleLinesEx(board, 5, BLACK);

        for (size_t i = 0; i < cells.count; ++i) {
          Cell c = cells.items[i];
          DrawRectangleLinesEx(c.rect, 3, black);
          if (c.is_dark)
              DrawRectangleRec(c.rect, black);
        }

        /*
        Rectangle src = { .x = 0, .y = 0, .width = CRAPPY_DIM, .height = CRAPPY_DIM };
        Rectangle dest = { .x = board.x, .y = board.y, .width = cell_dim*0.9, .height = cell_dim*0.9 };
        DrawTexturePro(crappy, src, dest, Vector2Zero(), 0, WHITE);
        */
        EndDrawing();
    }

    UnloadTexture(crappy);
    CloseWindow();
    return 0;
}
