#include <stddef.h>
#include <raylib.h>

#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "./nob.h"

#define BLACK_SQR_COLOR 0x1c1916ff

Rectangle DrawBoard() {
    size_t dim = GetScreenHeight() * 0.9;
    size_t x = GetScreenWidth() / 2 - dim / 2;
    size_t y = GetScreenHeight() / 2 - dim / 2;
    Rectangle r = { .x = x, .y = y, .width = dim, .height = dim};
    DrawRectangleLinesEx(r, 5, BLACK);
    return r;
}

void DrawCells(Rectangle board) {
    Color black = GetColor(BLACK_SQR_COLOR);
    size_t cell_dim = board.width / 8;
    for (size_t y = board.y; y < board.y + board.height - cell_dim; y += cell_dim) {
        for (size_t x = board.x; x < board.x + board.width - cell_dim; x += cell_dim) {
            Rectangle r = { .x = x, .y = y, .width = cell_dim, .height = cell_dim };
            DrawRectangleLinesEx(r, 3, black); 
            if ((y % 2 == 0 && x % 2 == 1) ||
                 (y % 2 == 1 && x % 2 == 0)) {
                DrawRectangleRec(r, black);
            }
        }
    }
}

int main(void)
{
    InitWindow(1920, 1080, "Chess");

    const char* crappy_png = "./assets/crappy/crappy.png";
    Texture2D crappy = LoadTexture(crappy_png);

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(GetColor(0xb77e38FF));

        Rectangle board = DrawBoard();
        DrawCells(board); 

        EndDrawing();
    }

    UnloadTexture(crappy);
    CloseWindow();
    return 0;
}
