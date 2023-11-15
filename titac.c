#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "raylib.h"

#define GRID_SIZE 9
#define GRID_SIDE 3
#define P1_WIN 3
#define P2_WIN -3

typedef enum {
    GridStateEmpty = 0,
    GridStatePlayer1 = 1,
    GridStatePlayer2 = -1,
} CellState;

typedef enum {
    GameStateWinP1,
    GameStateWinP2,
    GameStateDraw,
    GameStateContinue,
    GameStart,
} GameState;

typedef enum {
    PlayerTurn1,
    PlayerTurn2,
} PlayerTurn;

void draw_grid(const CellState grid[]);
size_t index_from_mouse(void);
size_t random_index(void);
void player_one_handler(CellState grid[]);
void player_two_ai_handler(CellState grid[]);
int is_grid_filled(const CellState grid[]);
GameState game_status(const CellState grid[]);

int main(void) {
    InitWindow(400, 400, "titac");

    const Color end_color = ColorAlpha(GRAY, 0.5);

    CellState grid[GRID_SIZE] = { GridStateEmpty };
    GameState game_state = GameStateContinue;
    PlayerTurn turn = PlayerTurn1;

    while (!WindowShouldClose()) {
        switch (GetKeyPressed()) {
            case KEY_ESCAPE:
                CloseWindow();
                break;

            case KEY_SPACE:
                if (game_state != GameStart) {
                    game_state = GameStart;
                }
                break;

            default:
                break;
        };

        switch (game_state) {
            case GameStateContinue:
                switch (turn) {
                    case PlayerTurn1:
                        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)
                            && grid[index_from_mouse()] == GridStateEmpty) {
                            player_one_handler(grid);
                            turn = PlayerTurn2;
                        }
                        break;

                    case PlayerTurn2:
                        player_two_ai_handler(grid);
                        turn = PlayerTurn1;
                        break;
                }
                game_state = game_status(grid);
                break;

            case GameStart:
                for (size_t i = 0; i < GRID_SIZE; i++) {
                    grid[i] = GridStateEmpty;
                }
                turn = PlayerTurn1;
                game_state = GameStateContinue;
                break;

            default:
                break;
        }

        BeginDrawing();

        ClearBackground(RAYWHITE);
        draw_grid(grid);

        switch (game_state) {
            case GameStateWinP1:
                DrawRectangleV(
                    (Vector2) { 0 },
                    (Vector2) {
                        GetScreenWidth(),
                        GetScreenHeight()
                    },
                    end_color
                );
                DrawText("P1 wins!\nPress SPACE to restart.", 0, 0, 16, BLACK);
                break;

            case GameStateWinP2:
                DrawRectangleV(
                    (Vector2) { 0 },
                    (Vector2) {
                        GetScreenWidth(),
                        GetScreenHeight(),
                    },
                    end_color
                );
                DrawText("P2 wins!\nPress SPACE to restart.", 0, 0, 16, BLACK);
                break;

            case GameStateDraw:
                DrawRectangleV(
                    (Vector2) { 0 },
                    (Vector2) {
                        GetScreenWidth(),
                        GetScreenHeight(),
                    },
                    end_color
                );
                DrawText("Draw!\nPress SPACE to restart.", 0, 0, 16, BLACK);
                break;

            default:
                break;
        }

        EndDrawing();
    }
}

GameState game_status(const CellState grid[]) {
    if (is_grid_filled(grid)) {
        return GameStateDraw;
    }

    int sums[8] = { 0 };
    for (size_t i = 0; i < GRID_SIDE; i++) {
        // Horizontal checksum
        sums[i] = grid[i * 3] + grid[i * 3 + 1] + grid[i * 3 + 2];
        // Vertical checksum
        sums[i + 3] = grid[i] + grid[i + 3] + grid[i + 6];
    }
    // Diagonal checksum
    sums[6] = grid[0] + grid[4] + grid[8];
    sums[7] = grid[2] + grid[4] + grid[6];

    for (size_t i = 0; i < 8; i++) {
        switch (sums[i]) {
            case P1_WIN:
                return GameStateWinP1;
            case P2_WIN:
                return GameStateWinP2;
        }
    }

    return GameStateContinue;
}

int is_grid_filled(const CellState grid[]) {
    for (size_t i = 0; i < GRID_SIZE; i++) {
        if (grid[i] == GridStateEmpty) {
            return 0;
        }
    }

    return 1;
}

void player_two_ai_handler(CellState grid[]) {
    int placed = 0;
    while (!placed && !is_grid_filled(grid)) {
        size_t idx = random_index();
        if (grid[idx] == GridStateEmpty) {
            grid[idx] = GridStatePlayer2;
            placed = 1;
        }
    }
}

void player_one_handler(CellState grid[]) {
    size_t idx = index_from_mouse();
    if (grid[idx] == GridStateEmpty) {
        grid[idx] = GridStatePlayer1;
    }
}

size_t random_index(void) {
    size_t x = GetRandomValue(0, GRID_SIDE - 1);
    size_t y = GetRandomValue(0, GRID_SIDE - 1);

    return y * GRID_SIDE + x;
}

size_t index_from_mouse(void) {
    int width_dist = GetScreenWidth() / 3;
    int height_dist = GetScreenHeight() / 3;
    Vector2 mouse_pos = GetMousePosition();
    size_t x = (size_t)mouse_pos.x / width_dist;
    size_t y = (size_t)mouse_pos.y / height_dist;

    return y * GRID_SIDE + x;
}

void draw_grid(const CellState grid[]) {
    int width_dist = GetScreenWidth() / 3;
    int height_dist = GetScreenHeight() / 3;
    int padding_top = 28 * GetScreenWidth() / GetScreenHeight();
    int padding_right = 38 * GetScreenWidth() / GetScreenHeight();

    for (size_t i = 0; i < 2; i++) {
        int dist_row = width_dist * (i + 1);
        int dist_col = height_dist * (i + 1);
        DrawLine(0, dist_row, GetScreenWidth(), dist_row, BLACK);
        DrawLine(dist_col, 0, dist_col, GetScreenHeight(), BLACK);
    }

    for (size_t x = 0; x < GRID_SIDE; x++) {
        for (size_t y = 0; y < GRID_SIDE; y++) {
            int dist_row = x * width_dist + padding_right;
            int dist_col = y * height_dist + padding_top;

            switch (grid[y * GRID_SIDE + x]) {
                case GridStateEmpty:
                    DrawText("-", dist_row, dist_col, 96, GRAY);
                    break;
                case GridStatePlayer1:
                    DrawText("O", dist_row, dist_col, 96, GRAY);
                    break;
                case GridStatePlayer2:
                    DrawText("X", dist_row, dist_col, 96, GRAY);
                    break;
            }
        }
    }
}
