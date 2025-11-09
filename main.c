#include <stddef.h>
#include <raylib.h>
#include <raymath.h>

#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "./nob.h"

//#define SW 1200
//#define SH 720
#define SW 1920
#define SH 1080
#define BLACK_SQR_COLOR 0x1c1916ff
#define LETTER_FONT_SIZE 40

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
  bool is_cell_dark;
  bool is_piece_dark;
  size_t row;
  size_t col;
  Piece piece;
} Cell;

typedef struct {
  Cell *items;
  size_t count;
  size_t capacity;
} Cells;

typedef struct {
    const char* file_path;
    Texture2D tex;
    size_t piece_dim;
    size_t y_white;
    size_t y_black;
    size_t rook;
    size_t horsey;
    size_t bishop;
    size_t queen;
    size_t king;
    size_t pawn;
} PiecesTexture;

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
            c->is_cell_dark = ((row % 2 == 0 && col % 2 == 1) || (row % 2 == 1 && col % 2 == 0));
            c->is_piece_dark = row == 6 || row == 7;
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

void DrawPiece(Cell c, PiecesTexture p) {
    size_t src_y = c.is_piece_dark ? p.y_black : p.y_white;
    size_t src_x = 0;
    switch (c.piece) {
        case None: return;
        case Rook: src_x = p.rook; break;
        case Knight: src_x = p.horsey; break;
        case Bishop: src_x = p.bishop; break;
        case Queen: src_x = p.queen; break;
        case King: src_x = p.king; break;
        case Pawn: src_x = p.pawn; break;
    }
    size_t pc_dim = c.rect.width*0.9;
    size_t padding = c.rect.width - pc_dim;
    Rectangle src = { .x = src_x, .y = src_y, .width = p.piece_dim, .height = p.piece_dim};
    Rectangle dest = { .x = c.rect.x + padding/2, .y = c.rect.y + padding/2, .width = pc_dim, .height = pc_dim };
    DrawTexturePro(p.tex, src, dest, Vector2Zero(), 0, WHITE);
}

PiecesTexture GetPiecesTexture(const char* name) { 
    PiecesTexture pieces = {0};
    
    if (strcmp(name, "crappy") == 0) {
        pieces.file_path = "./assets/crappy/crappy.png";
        pieces.tex = LoadTexture(pieces.file_path);
        pieces.y_white = 0;
        pieces.y_black = 100;
        pieces.piece_dim = 100;
        pieces.rook = 0;
        pieces.horsey = 100;
        pieces.bishop = 200;
        pieces.queen = 300;
        pieces.king = 400;
        pieces.pawn = 500;
    }

    return pieces;
}

void DrawBoard(Rectangle board, size_t pc_dim) {
    DrawRectangleLinesEx(board, 5, BLACK);
    for (size_t i = 1; i <= 8; ++i) {
        size_t x = board.x + ((i-1) * pc_dim) + (pc_dim/2) - MeasureText("h", LETTER_FONT_SIZE)/2;
        size_t y = board.y - LETTER_FONT_SIZE - 5;
        switch (i) {
            case 1: DrawText("a", x, y, LETTER_FONT_SIZE, BLACK); break;
            case 2: DrawText("b", x, y, LETTER_FONT_SIZE, BLACK); break;
            case 3: DrawText("c", x, y, LETTER_FONT_SIZE, BLACK); break;
            case 4: DrawText("d", x, y, LETTER_FONT_SIZE, BLACK); break;
            case 5: DrawText("e", x, y, LETTER_FONT_SIZE, BLACK); break;
            case 6: DrawText("f", x, y, LETTER_FONT_SIZE, BLACK); break;
            case 7: DrawText("g", x, y, LETTER_FONT_SIZE, BLACK); break;
            case 8: DrawText("h", x, y, LETTER_FONT_SIZE, BLACK); break;
        }
    }
    for (size_t i = 1; i <= 8; ++i) {
        size_t x = board.x - MeasureText("8", LETTER_FONT_SIZE)*2;
        size_t y = board.y + ((i-1) * pc_dim) + (pc_dim/2);
        switch(i) {
            case 1: DrawText("1", x, y, LETTER_FONT_SIZE, BLACK); break;
            case 2: DrawText("2", x, y, LETTER_FONT_SIZE, BLACK); break;
            case 3: DrawText("3", x, y, LETTER_FONT_SIZE, BLACK); break;
            case 4: DrawText("4", x, y, LETTER_FONT_SIZE, BLACK); break;
            case 5: DrawText("5", x, y, LETTER_FONT_SIZE, BLACK); break;
            case 6: DrawText("6", x, y, LETTER_FONT_SIZE, BLACK); break;
            case 7: DrawText("7", x, y, LETTER_FONT_SIZE, BLACK); break;
            case 8: DrawText("8", x, y, LETTER_FONT_SIZE, BLACK); break;

        }
    }
}

int main(void)
{
    InitWindow(SW, SH, "Chess");
    
    PiecesTexture pieces = GetPiecesTexture("crappy");

    Cells cells = {0};
    Rectangle board = CreateBoard();
    CreateCells(board, &cells);
    Color black = GetColor(BLACK_SQR_COLOR);

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(GetColor(0xb77e38FF));

        DrawBoard(board, cells.items[0].rect.width);

        for (size_t i = 0; i < cells.count; ++i) {
          Cell c = cells.items[i];
          DrawRectangleLinesEx(c.rect, 3, black);
          if (c.is_cell_dark) {
              DrawRectangleRec(c.rect, black);
          }
          DrawPiece(c, pieces);
        }

        EndDrawing();
    }

    UnloadTexture(pieces.tex);
    CloseWindow();
    return 0;
}
