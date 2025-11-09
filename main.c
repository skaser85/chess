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

Rectangle DrawBoard() {
    size_t dim = GetScreenHeight() * 0.9;
    size_t x = GetScreenWidth() / 2 - dim / 2;
    size_t y = GetScreenHeight() / 2 - dim / 2;
    Rectangle r = { .x = x, .y = y, .width = dim, .height = dim};
    DrawRectangleLinesEx(r, 5, BLACK);
    return r;
}

size_t DrawCells(Rectangle board) {
    Color black = GetColor(BLACK_SQR_COLOR);
    size_t cell_dim = board.width / 8;
    for (size_t y = board.y; y < board.y + board.height ; y += cell_dim) {
        for (size_t x = board.x; x < board.x + board.width ; x += cell_dim) {
            Rectangle r = { .x = x, .y = y, .width = cell_dim, .height = cell_dim };
            DrawRectangleLinesEx(r, 3, black); 
            if ((y % 2 == 0 && x % 2 == 1) ||
                 (y % 2 == 1 && x % 2 == 0)) {
                DrawRectangleRec(r, black);
            }
        }
    }
    return cell_dim;
}

int main(void)
{
    InitWindow(SW, SH, "Chess");

    const char* crappy_png = "./assets/crappy/crappy.png";
    Texture2D crappy = LoadTexture(crappy_png);

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(GetColor(0xb77e38FF));

        Rectangle board = DrawBoard();
        size_t cell_dim = DrawCells(board); 

        Rectangle src = { .x = 0, .y = 0, .width = CRAPPY_DIM, .height = CRAPPY_DIM };
        Rectangle dest = { .x = board.x, .y = board.y, .width = cell_dim*0.9, .height = cell_dim*0.9 };
        DrawTexturePro(crappy, src, dest, Vector2Zero(), 0, WHITE);

        EndDrawing();
    }

    UnloadTexture(crappy);
    CloseWindow();
    return 0;
}
