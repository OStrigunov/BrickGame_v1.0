#ifndef TETRIS_H
#define TETRIS_H

#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "../../gui/cli/frontend_tetris.h"
#include "backend/backend_tetris.h"
#include "backend/figures.h"

void main_game_loop();

#endif