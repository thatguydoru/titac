#include <stddef.h>
#include <stdio.h>
#include <threads.h>
#include <time.h>

#include "raylib.h"

#define GRID_SIZE 9
#define GRID_SIDE 3
#define P1_WIN 3
#define P2_WIN -3
#define CELL_WIDTH (GetScreenWidth() / (float)GRID_SIDE)
#define CELL_HEIGHT (GetScreenHeight() / (float)GRID_SIDE)

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
int player_one_handler(CellState grid[]);
int player_two_ai_handler(CellState grid[]);
int is_grid_filled(const CellState grid[]);
GameState game_status(const CellState grid[]);
void draw_win_line(const CellState grid[]);

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
                game_state = GameStart;
                break;

            default:
                break;
        };

        switch (game_state) {
            case GameStateContinue:
                switch (turn) {
                    case PlayerTurn1:
                        if (player_one_handler(grid)) {
                            turn = PlayerTurn2;
                        }
                        break;

                    case PlayerTurn2:
                        if (player_two_ai_handler(grid)) {
                            turn = PlayerTurn1;
                        }
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
                draw_win_line(grid);
                DrawRectangleV(
                    (Vector2) { 0 },
                    (Vector2) {
                        GetScreenWidth(),
                        GetScreenHeight()
                    },
                    end_color
                );
                DrawText(
                    "P1 wins!\nSPACE: restart\nESC: quit",
                    0,
                    0,
                    16,
                    BLACK
                );
                break;

            case GameStateWinP2:
                draw_win_line(grid);
                DrawRectangleV(
                    (Vector2) { 0 },
                    (Vector2) {
                        GetScreenWidth(),
                        GetScreenHeight(),
                    },
                    end_color
                );
                DrawText(
                    "P2 wins!\nSPACE: restart\nESC: quit",
                    0,
                    0,
                    16,
                    BLACK
                );
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
                DrawText(
                    "Draw!\nSPACE: restart\nESC: quit",
                    0,
                    0,
                    16,
                    BLACK
                );
                break;

            default:
                break;
        }

        EndDrawing();
    }
}

void draw_win_line(const CellState grid[]) {
    Vector2 start = { 0 };
    Vector2 end = { 0 };

    for (size_t i = 0; i < GRID_SIDE; i++) {
        if (grid[i * 3] != GridStateEmpty
            && grid[i * 3] == grid[i * 3 + 1]
            && grid[i * 3] == grid[i * 3 + 2]) {
            start.x = 0;
            start.y = (i * 2 + 1) * CELL_HEIGHT / 2;
            end.x = GetScreenWidth();
            end.y = (i * 2 + 1) * CELL_HEIGHT / 2;
            break;
        }

        if (grid[i] != GridStateEmpty
            &&grid[i] == grid[i + 3]
            && grid[i] == grid[i + 6]) {
            start.x = (i * 2 + 1) *  CELL_WIDTH / 2.;
            start.y = 0;
            end.x = (i * 2 + 1) * CELL_WIDTH / 2;
            end.y = GetScreenHeight();
            break;
        }
    }

    if (grid[0] != GridStateEmpty
        && grid[0] == grid[4]
        && grid[0] == grid[8]) {
        start.x = 0;
        start.y = 0;
        end.x = GetScreenWidth();
        end.y = GetScreenHeight();
    } else if (grid[2] != GridStateEmpty
        && grid[2] == grid[4]
        && grid[2] == grid[6]) {
        start.x = GetScreenWidth();
        start.y = 0;
        end.x = 0;
        end.y = GetScreenHeight();
    }

    DrawLineEx(start, end, 20, RED);
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

int player_two_ai_handler(CellState grid[]) {
    int placed = 0;
    while (!placed && !is_grid_filled(grid)) {
        size_t idx = random_index();
        if (grid[idx] == GridStateEmpty) {
            grid[idx] = GridStatePlayer2;
            placed = 1;
        }
    }

    thrd_sleep(&(struct timespec) { .tv_nsec = 80000000 }, NULL);

    return 1;
}

int player_one_handler(CellState grid[]) {
    size_t idx = index_from_mouse();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)
        && grid[idx] == GridStateEmpty) {
        grid[idx] = GridStatePlayer1;
        return 1;
    }

    return 0;
}

size_t random_index(void) {
    size_t x = GetRandomValue(0, GRID_SIDE - 1);
    size_t y = GetRandomValue(0, GRID_SIDE - 1);

    return y * GRID_SIDE + x;
}

size_t index_from_mouse(void) {
    Vector2 mouse_pos = GetMousePosition();
    size_t x = (size_t)mouse_pos.x / CELL_WIDTH;
    size_t y = (size_t)mouse_pos.y / CELL_HEIGHT;

    return y * GRID_SIDE + x;
}

void draw_grid(const CellState grid[]) {
    int padding_top = 28 * GetScreenWidth() / GetScreenHeight();
    int padding_right = 38 * GetScreenWidth() / GetScreenHeight();

    for (size_t i = 1; i < GRID_SIDE; i++) {
        int dist_row = CELL_WIDTH * i;
        int dist_col = CELL_HEIGHT * i;
        DrawLine(0, dist_row, GetScreenWidth(), dist_row, BLACK);
        DrawLine(dist_col, 0, dist_col, GetScreenHeight(), BLACK);
    }

    for (size_t x = 0; x < GRID_SIDE; x++) {
        for (size_t y = 0; y < GRID_SIDE; y++) {
            int dist_row = x * CELL_WIDTH + padding_right;
            int dist_col = y * CELL_HEIGHT + padding_top;

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
