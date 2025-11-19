#include <stddef.h>
#include <raylib.h>
#include <raymath.h>

#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "../nob.h"

#define ASSETS_DIR "./assets/"

//#define SW 1200
//#define SH 720
#define SW 1920
#define SH 1080
#define BLACK_SQR_COLOR 0x1c1916ff
#define LETTER_FONT_SIZE 40

typedef struct Cell Cell;
typedef struct Cells Cells;

typedef enum {
  None,
  Pawn,
  Rook,
  Knight,
  Bishop,
  Queen,
  King,
} PieceKind;

typedef struct {
  PieceKind kind;
  bool is_dark;
  size_t value;
  bool first_move_made;
  Cells* valid_moves;
} Piece;

typedef struct {
    Piece* items;
    size_t count;
    size_t capacity;
} Pieces;

struct Cell {
  Rectangle rect;
  bool is_dark;
  size_t row;
  size_t col;
  bool is_active;
  Rectangle active_rect;
  Piece* piece;
};

struct Cells {
  Cell *items;
  size_t count;
  size_t capacity;
};

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

size_t GetPieceValue(PieceKind p) {
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
            c->is_active = false;
            float scale = 0.98;
            float padding = c->rect.width - (c->rect.width*scale);
            Rectangle ar = { 
                .x = c->rect.x + padding, 
                .y = c->rect.y + padding, 
                .width = c->rect.width-(padding*2), 
                .height = c->rect.height-(padding*2) 
            };
            c->active_rect = ar;
            c->piece = (Piece*)malloc(sizeof(Piece));
            c->piece->kind = None;
            c->piece->valid_moves = malloc(sizeof(Cells));
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

void CreatePieces(Cells* cells) {
    for (size_t row = 0; row < 2; ++row) {
        for (size_t col = 0; col < 8; ++col) {
            Cell* cell = GetCell(cells, row, col);
            if (!cell) {
              nob_log(ERROR, "Could not find a Cell at row %ld col %ld!", row, col);
              return;
            }
            Piece* p = cell->piece;
            p->is_dark = false;
            p->kind = None;
            if (row == 0) {
                if (col == 0 || col == 7) {
                    p->kind = Rook;
                } else if (col == 1 || col == 6) {
                    p->kind = Knight;
                } else if (col == 2 || col == 5) {
                    p->kind = Bishop;
                } else if (col == 3) {
                    p->kind = King;
                } else {
                    p->kind = Queen;
                }
            } else {
                p->kind = Pawn;
            }
        }
    }
    
    for (size_t row = 7; row > 5; --row) {
        for (size_t col = 0; col < 8; ++col) {
            Cell* cell = GetCell(cells, row, col);
            if (!cell) {
              nob_log(ERROR, "Could not find a Cell at row %ld col %ld!", row, col);
              return;
            }
            Piece* p = cell->piece; 
            p->is_dark = true;
            p->kind = None;
            if (row == 7) {
                if (col == 0 || col == 7) {
                    p->kind = Rook;
                } else if (col == 1 || col == 6) {
                    p->kind = Knight;
                } else if (col == 2 || col == 5) {
                    p->kind = Bishop;
                } else if (col == 3) {
                    p->kind = King;
                } else {
                    p->kind = Queen;
                }
            } else {
                p->kind = Pawn;
            }
        }
    }
}

PieceTexture GetPieceTexture(const char* kind) { 
    PieceTexture pieces = {0};
    
    if (strcmp(kind, "derpy") == 0) {
        pieces.file_path = ASSETS_DIR"derpy/derpy.png";
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
    } else if (strcmp(kind, "pixelated") == 0) {
        pieces.file_path = ASSETS_DIR"pixelated/pixelated.png";
        pieces.tex = LoadTexture(pieces.file_path);
        pieces.y_white = 64;
        pieces.y_black = 0;
        pieces.piece_dim = 64;
        pieces.rook = 0;
        pieces.horsey = 64;
        pieces.bishop = 128;
        pieces.queen = 192;
        pieces.king = 256;
        pieces.pawn = 320;
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

void DrawCell(Cell* c, Vector2 mouse, bool pressed, Cell* active_cell) {
    Color black = GetColor(BLACK_SQR_COLOR);
    Color color = black;
    if (CheckCollisionPointRec(mouse, c->rect)) {
        if (pressed) {
            if (c->is_active) {
                c->is_active = false;
            } else {
                if (active_cell && c != active_cell) {
                    Piece* temp = active_cell->piece;
                    active_cell->piece = c->piece;
                    c->piece = temp;
                    active_cell->is_active = false;
                    active_cell = NULL;
                } else {
                    c->is_active = true;
                    active_cell = c;
                }
            }
        }
        color = LIME;
    }
    if (c->is_active)
        DrawRectangleRec(c->active_rect, PINK);
    else if (c->is_dark) 
      DrawRectangleRec(c->rect, black); 
    DrawRectangleLinesEx(c->rect, 3, color);
}

void AddValidMoves(Cells* cells, Cell* c) {
    c->piece->valid_moves->count = 0;
    switch (c->piece->kind) {
        case None: return;
        case Pawn: {
            if (c->row < 7) {
                Cell* v = GetCell(cells, c->row+1, c->col);
                if (!v) {
                    nob_log(ERROR, "Could not get a cell at row %ld and col %ld", c->row+1, c->col);
                    exit(1);
                }
                da_append(c->piece->valid_moves, *v);
                if (!c->piece->first_move_made) {
                    v = GetCell(cells, c->row+2, c->col);
                    if (!v) {
                        nob_log(ERROR, "Could not get a cell at row %ld and col %ld", c->row+2, c->col);
                        exit(1);
                    }
                    da_append(c->piece->valid_moves, *v);
                }
            }
        } break;
        default: return;
    }
}


void DrawPiece(Cell* c, PieceTexture t) {
    size_t src_y = c->piece->is_dark ? t.y_black : t.y_white;
    size_t src_x = 0;
    switch (c->piece->kind) {
        case None: return;
        case Rook: src_x = t.rook; break;
        case Knight: src_x = t.horsey; break;
        case Bishop: src_x = t.bishop; break;
        case Queen: src_x = t.queen; break;
        case King: src_x = t.king; break;
        case Pawn: src_x = t.pawn; break;
    }
    
    size_t pc_dim = c->rect.width*0.9;
    size_t padding = c->rect.width - pc_dim;
    
    Rectangle src = { .x = src_x, .y = src_y, .width = t.piece_dim, .height = t.piece_dim};
    Rectangle dest = { .x = c->rect.x+(padding/2), .y = c->rect.y+(padding/2), .width = pc_dim, .height = pc_dim };
    
    DrawTexturePro(t.tex, src, dest, Vector2Zero(), 0, WHITE);
}

int main(void)
{

    InitWindow(SW, SH, "Chess");

    //PieceTexture pieceTexture = GetPieceTexture("derpy");
    PieceTexture pieceTexture = GetPieceTexture("pixelated");
    Rectangle board = CreateBoard();
    
    Cells cells = {0};
    CreateCells(board, &cells);
    CreatePieces(&cells);

    Cell* active_cell = NULL;

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(GetColor(0xb77e38FF));

        DrawBoard(board, cells.items[0].rect.width);

        Vector2 mouse = GetMousePosition();
        bool left_mouse_button_pressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

        for (size_t i = 0; i < cells.count; ++i) {
          Cell* c = &cells.items[i];
          DrawCell(c, mouse, left_mouse_button_pressed, active_cell);
          if (c->is_active) {
              active_cell = c;
              if (c->piece->valid_moves)
                  free(c->piece->valid_moves);
              AddValidMoves(&cells, c);
              if (c->piece->valid_moves && c->piece->valid_moves->count > 0) {
                for (size_t m = 0; m < c->piece->valid_moves->count; ++m) {
                    Cell v = c->piece->valid_moves->items[m];
                    DrawRectangleRec(v.active_rect, BLUE);
                }
              }
          } else {
            if (c == active_cell)
                active_cell = NULL;
          }
          DrawPiece(c, pieceTexture);
        }

        EndDrawing();
    }

    UnloadTexture(pieceTexture.tex);
    CloseWindow();
    return 0;
}
