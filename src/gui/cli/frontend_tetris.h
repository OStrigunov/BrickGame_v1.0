#ifndef FRONTEND_TETRIS_H
#define FRONTEND_TETRIS_H

#include <ncurses.h>

#include "../../brick_game/tetris/backend/backend_tetris.h"
#include "../../brick_game/tetris/tetris.h"

// начальные координаты игрового поля на экране
#define F_Y_START 2
#define F_X_START 3

// вид и длина клетки фигурки
#define CELL "[]"
#define CELL_SIZE strlen(CELL)

// индексы кастомных цветов
#define COLOR_ORANGE 8
#define COLOR_CUSTOM_YELLOW 9
#define COLOR_VIOLET 10
#define COLOR_CUSTOM_MAGENTA 11

// цветовые пары
#define RED_P 1
#define ORANGE_P 8
#define YELLOW_P 9
#define GREEN_P 2
#define BLUE_P 4
#define VIOLET_P 10
#define MAGENTA_P 11

void init_ncurses();
void init_colors();

void print_outer_frame();
void print_playing_field_frame();
void print_box(int top_y, int bottom_y, int left_x, int right_x);

void print_field(GameInfo_t game);
void print_figure(Tetramino figure);
void print_next_figure(Tetramino figure, int y, int x);

void print_statistic(GameInfo_t game);

void print_game_screen(GameInfo_t game);
void print_start_screen();
void print_pause();
void print_game_over(GameInfo_t game);

#endif  // FRONTEND_TETRIS_H
