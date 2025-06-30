#ifndef FIGURES_TETRIS_H
#define FIGURES_TETRIS_H

#include "backend_tetris.h"
#include "figures.h"

void reset_figure(Tetramino *figure);
void generate_figure(Tetramino *figure);
void rotate_figure();
void spawn_figure();
void move_left();
void move_right();
void move_down();
void attached_figure();

#endif  // FIGURES_TETRIS_H