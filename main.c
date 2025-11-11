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
} PieceName;

typedef struct {
  Rectangle rect;
  bool is_dark;
  size_t row;
  size_t col;
  bool is_active;
} Cell;

typedef struct {
  PieceName name;
  bool is_dark;
  size_t value;
  Vector2 pos;
  size_t dim;
  bool is_dragging;
  Cell* cell;
} Piece;

typedef struct {
    Piece* items;
    size_t count;
    size_t capacity;
} Pieces;

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
} PieceTexture;

size_t GetPieceValue(PieceName p) {
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
            da_append(cells, *c);
        }
    }
}

Cell* GetCell(Cells* cells, size_t row, size_t col) {
    for (size_t i = 0; i < cells->count; ++i) {
      Cell* c = &cells->items[i];
      if (c->row == row && c->col == col)
          return c;
    }
    return NULL;
}

void CreatePieces(Pieces* pieces, Cells* cells) {
    for (size_t row = 0; row < 2; ++row) {
        for (size_t col = 0; col < 8; ++col) {
            Piece* p = malloc(sizeof(Piece));
            p->is_dragging = 0;
            p->is_dark = false;
            p->name = None;
            if (row == 0) {
                if (col == 0 || col == 7) {
                    p->name = Rook;
                } else if (col == 1 || col == 6) {
                    p->name = Knight;
                } else if (col == 2 || col == 5) {
                    p->name = Bishop;
                } else if (col == 3) {
                    p->name = King;
                } else {
                    p->name = Queen;
                }
            } else {
                p->name = Pawn;
            }
            Cell* cell = GetCell(cells, row, col);
            if (!cell) {
              nob_log(ERROR, "Could not find a Cell at row %ld col %ld!", row, col);
              return;
            } 
            p->cell = cell;
            p->dim = cell->rect.width*0.9;
            size_t padding = cell->rect.width - p->dim;
            Vector2* v = malloc(sizeof(Vector2));
            v->x = cell->rect.x + padding/2;
            v->y = cell->rect.y + padding/2;
            p->pos = *v;
            da_append(pieces, *p);
        }
    }
    
    for (size_t row = 7; row > 5; --row) {
        for (size_t col = 0; col < 8; ++col) {
            Piece* p = malloc(sizeof(Piece));
            p->is_dragging = false;
            p->is_dark = true;
            p->name = None;
            if (row == 7) {
                if (col == 0 || col == 7) {
                    p->name = Rook;
                } else if (col == 1 || col == 6) {
                    p->name = Knight;
                } else if (col == 2 || col == 5) {
                    p->name = Bishop;
                } else if (col == 3) {
                    p->name = King;
                } else {
                    p->name = Queen;
                }
            } else { p->name = Pawn;
            }
            Cell* cell = GetCell(cells, row, col);
            if (!cell) {
              nob_log(ERROR, "Could not find a Cell at row %ld col %ld!", row, col);
              return;
            } 
            p->cell = cell;
            p->dim = cell->rect.width*0.9;
            size_t padding = cell->rect.width - p->dim;
            Vector2* v = malloc(sizeof(Vector2));
            v->x = cell->rect.x + padding/2;
            v->y = cell->rect.y + padding/2;
            p->pos = *v;
            da_append(pieces, *p);
        }
    }
}

void DrawPiece(Piece* p, PieceTexture t, Vector2 mouse) {
    size_t src_y = p->is_dark ? t.y_black : t.y_white;
    size_t src_x = 0;
    switch (p->name) {
        case None: return;
        case Rook: src_x = t.rook; break;
        case Knight: src_x = t.horsey; break;
        case Bishop: src_x = t.bishop; break;
        case Queen: src_x = t.queen; break;
        case King: src_x = t.king; break;
        case Pawn: src_x = t.pawn; break;
    }
    
    size_t pc_dim = p->cell->rect.width*0.9;
    size_t padding = p->cell->rect.width - pc_dim;
    
    bool pressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    Rectangle r = { .x = p->pos.x, .y = p->pos.y, .width = t.piece_dim, .height = t.piece_dim };
    bool hovered = CheckCollisionPointRec(mouse, r);
    if (pressed && p->cell->row == 0 && p->cell->col == 0) {
        nob_log(INFO, "mouse button left pressed %d", p->name);
        nob_log(INFO, "cell->is_active %b %ld %ld", p->cell->is_active, p->cell->row, p->cell->col);
        nob_log(INFO, "p->is_dragging %b", p->is_dragging);
    }
    if (hovered && pressed && !p->is_dragging) {
        p->is_dragging = true;
    }
    if (p->is_dragging) {
      if (pressed) {
        p->is_dragging = false;
      }
    }
    if (p->is_dragging) {
      nob_log(INFO, "dragging %d", p->name);
      p->pos.x = mouse.x;
      p->pos.y = mouse.y;
    } else {
        p->pos.x = p->cell->rect.x + padding/2;
        p->pos.y = p->cell->rect.y + padding/2;
    }
    
    Rectangle src = { .x = src_x, .y = src_y, .width = t.piece_dim, .height = t.piece_dim};
    Rectangle dest = { .x = p->pos.x, .y = p->pos.y, .width = pc_dim, .height = pc_dim };
    
    DrawTexturePro(t.tex, src, dest, Vector2Zero(), 0, WHITE);
}

PieceTexture GetPieceTexture(const char* name) { 
    PieceTexture pieces = {0};
    
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

void DrawCell(Cell* c, Vector2 mouse) {
    Color black = GetColor(BLACK_SQR_COLOR);
    DrawRectangleLinesEx(c->rect, 3, black);
    c->is_active = CheckCollisionPointRec(mouse, c->rect);
    if (c->is_active) {
        float scale = 0.98;
        float padding = c->rect.width - (c->rect.width*scale);
        Rectangle r = { 
            .x = c->rect.x + padding, 
            .y = c->rect.y + padding, 
            .width = c->rect.width-(padding*2), 
            .height = c->rect.height-(padding*2) 
        };
        DrawRectangleRec(r, LIME);
        if (c->row == 0 && c->col == 0)
            nob_log(INFO, "active %ld %ld", c->row, c->col);
    } else if (c->is_dark) {
        DrawRectangleRec(c->rect, black);
    }
    
}

int main(void)
{

    InitWindow(SW, SH, "Chess");

    PieceTexture pieceTexture = GetPieceTexture("crappy");

    Rectangle board = CreateBoard();
    
    Cells cells = {0};
    CreateCells(board, &cells);
    
    Pieces pieces = {0};
    CreatePieces(&pieces, &cells);


    Piece* active_piece = NULL;

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(GetColor(0xb77e38FF));

        DrawBoard(board, cells.items[0].rect.width);

        Vector2 mouse = GetMousePosition();

        for (size_t i = 0; i < cells.count; ++i) {
          Cell c = cells.items[i];
          DrawCell(&c, mouse);
        }

        for (size_t i = 0; i < pieces.count; ++i) {
            Piece p = pieces.items[i];
            DrawPiece(&p, pieceTexture, mouse);
        }

        EndDrawing();
    }

    UnloadTexture(pieceTexture.tex);
    CloseWindow();
    return 0;
}
