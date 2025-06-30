/**
 * @file frontend.c
 * @brief Реализация модуля отображения игры Tetris (ncurses)
 * @ingroup frontend_module
 * @{
 */

#include "frontend_tetris.h"

/**
 * @brief Инициализация библиотеки ncurses
 */
void init_ncurses() {
  initscr();    // инициализируем библиотеку ncurses
  cbreak();     // при вводе не нужно ждать enter
  noecho();     // не отображать символы на экране
  curs_set(0);  // курсор невидимый
  keypad(stdscr, TRUE);  // обрабатывать специальные клавиши
  srand(time(NULL));  // инициализация генератора случайных чисел для фигур
  nodelay(stdscr, TRUE);  // включает неблокирующий ввод — getch() не ждёт ввода
  init_colors();
}

/**
 * @brief Основная функция отрисовки игрового экрана
 * @param game Текущее состояние игры
 */
void print_game_screen(GameInfo_t game) {
  print_outer_frame();
  switch (game.state) {
    case START:
      print_start_screen();
      break;
    case PAUSE:
      print_playing_field_frame();
      print_pause();
      print_field(game);
      print_statistic(game);
      print_figure(game.current);
      break;
    case GAMEOVER:
      print_playing_field_frame();
      print_field(game);
      print_figure(game.current);
      print_game_over(game);
      break;
    default:
      print_playing_field_frame();
      print_field(game);
      print_statistic(game);
      print_figure(game.current);
      break;
  }
}

/**
 * @brief Отрисовка рамки игрового поля
 */
void print_playing_field_frame() {
  print_box(1, F_Y_START + HEIGHT, 2, F_X_START + WIDTH * CELL_SIZE);
}

/**
 * @brief Отрисовка внешней рамки игры
 * @details Создает большую рамку вокруг всего игрового интерфейса
 */
void print_outer_frame() {
  print_box(0, F_Y_START + HEIGHT + 1, 0,
            F_X_START + (WIDTH * CELL_SIZE) * 2 + 6);
}

/**
 * @brief Универсальная функция отрисовки прямоугольной рамки
 */
void print_box(int top_y, int bottom_y, int left_x, int right_x) {
  for (int i = top_y + 1; i < bottom_y; i++) {
    mvaddch(i, left_x, ACS_VLINE);
    mvaddch(i, right_x, ACS_VLINE);
  }
  for (int i = left_x + 1; i < right_x; i++) {
    mvaddch(top_y, i, ACS_HLINE);
    mvaddch(bottom_y, i, ACS_HLINE);
  }
  mvaddch(top_y, left_x, ACS_ULCORNER);
  mvaddch(top_y, right_x, ACS_URCORNER);
  mvaddch(bottom_y, left_x, ACS_LLCORNER);
  mvaddch(bottom_y, right_x, ACS_LRCORNER);
}

/**
 * @brief Отрисовка стартового экрана
 */
void print_start_screen() {
  int x_offset = ((F_X_START + (WIDTH * CELL_SIZE) * 2 + 6) - 45) / 2 + 1;
  attron(COLOR_PAIR(BLUE_P) | A_BOLD);
  mvprintw(8, x_offset, " _______ ______ _______ _____  _____  _____ ");
  mvprintw(9, x_offset, "|__   __|  ____|__   __|  __ \\|_   _|/ ____|");
  mvprintw(10, x_offset, "   | |  | |__     | |  | |__) | | | | (___  ");
  mvprintw(11, x_offset, "   | |  |  __|    | |  |  _  /  | |  \\___ \\ ");
  mvprintw(12, x_offset, "   | |  | |____   | |  | | \\ \\ _| |_ ____) |");
  mvprintw(13, x_offset, "   |_|  |______|  |_|  |_|  \\_\\_____|_____/ ");
  attroff(COLOR_PAIR(BLUE_P) | A_BOLD);
  attron(A_BLINK);
  mvprintw(HEIGHT, (F_X_START + (WIDTH * CELL_SIZE) * 2 + 6) / 2 - 9,
           "ENTER - start game");
  mvprintw(HEIGHT + 1, (F_X_START + (WIDTH * CELL_SIZE) * 2 + 6) / 2 - 9,
           "    q - exit");
  attroff(A_BLINK);
}

/**
 * @brief Отрисовка игрового поля
 */
void print_field(GameInfo_t game) {
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      if (game.field[i][j] != 0) {
        attron(COLOR_PAIR(game.field[i][j]));
        mvprintw(F_Y_START + i, F_X_START + j * CELL_SIZE, CELL);
        attroff(COLOR_PAIR(game.field[i][j]));
      }
    }
  }
}

/**
 * @brief Отрисовка текущей фигуры
 * @param figure Текущая активная фигура
 * @details Отображает фигуру с учетом ее текущей позиции
 * и не отображает части фигуры, находящиеся за верхней границей поля
 */
void print_figure(Tetramino figure) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (figure.view[i][j] != 0 && figure.y + i >= 0) {
        attron(COLOR_PAIR(figure.view[i][j]));
        mvprintw(F_Y_START + figure.y + i,
                 F_X_START + figure.x * CELL_SIZE + j * CELL_SIZE, CELL);
        attroff(COLOR_PAIR(figure.view[i][j]));
      }
    }
  }
}

/**
 * @brief Отрисовка следующей фигуры
 */
void print_next_figure(Tetramino figure, int y, int x) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (figure.view[i][j] != 0) {
        attron(COLOR_PAIR(figure.view[i][j]));
        mvprintw(y + i, x + j * CELL_SIZE, CELL);
        attroff(COLOR_PAIR(figure.view[i][j]));
      }
    }
  }
}

/**
 * @brief Отрисовка игровой статистики и клавиши управления
 */
void print_statistic(GameInfo_t game) {
  mvprintw(F_Y_START, F_X_START + WIDTH * CELL_SIZE + 3, "SCORE: %d",
           game.score);
  mvprintw(F_Y_START + 2, F_X_START + WIDTH * CELL_SIZE + 3, "HIGH SCORE: %d",
           game.high_score);
  mvprintw(F_Y_START + 4, F_X_START + WIDTH * CELL_SIZE + 3, "LEVEL: %d",
           game.level);
  mvprintw(F_Y_START + 6, F_X_START + WIDTH * CELL_SIZE + 3, "NEXT:");
  print_next_figure(game.next, F_Y_START + 8,
                    F_X_START + WIDTH * CELL_SIZE + 3);
  mvprintw(F_Y_START + 15, F_X_START + WIDTH * CELL_SIZE + 3, "<   >  -  move");
  mvprintw(F_Y_START + 16, F_X_START + WIDTH * CELL_SIZE + 3, "  V    -  drop");
  mvprintw(F_Y_START + 17, F_X_START + WIDTH * CELL_SIZE + 3,
           "SPACE  -  rotate");
  mvprintw(F_Y_START + 18, F_X_START + WIDTH * CELL_SIZE + 3,
           "  p    -  pause");
  mvprintw(F_Y_START + 19, F_X_START + WIDTH * CELL_SIZE + 3, "  q    -  exit");
}

/**
 * @brief Отрисовка экрана паузы
 */
void print_pause() {
  attron(COLOR_PAIR(RED_P));
  mvprintw(F_Y_START + 12, F_X_START + WIDTH * CELL_SIZE + 3, "PAUSE");
  attroff(COLOR_PAIR(RED_P));
}

/**
 * @brief Отрисовка экрана завершения игры
 */
void print_game_over(GameInfo_t game) {
  mvprintw(F_Y_START, F_X_START + WIDTH * CELL_SIZE + 3, "SCORE: %d",
           game.score);
  mvprintw(F_Y_START + 2, F_X_START + WIDTH * CELL_SIZE + 3, "HIGH SCORE: %d",
           game.high_score);
  attron(COLOR_PAIR(RED_P));
  mvprintw(6, F_X_START + WIDTH * CELL_SIZE + 3, "[ GAME OVER ]");
  attroff(COLOR_PAIR(RED_P));
  attron(COLOR_PAIR(ORANGE_P));
  mvprintw(8, F_X_START + WIDTH * CELL_SIZE + 3, "BETTER LUCK NEXT TIME!");
  attroff(COLOR_PAIR(ORANGE_P));
  attron(COLOR_PAIR(GREEN_P));
  mvprintw(10, F_X_START + WIDTH * CELL_SIZE + 3, "TRY AGAIN?");
  attroff(COLOR_PAIR(GREEN_P));
  mvprintw(12, F_X_START + WIDTH * CELL_SIZE + 3, "ENTER  -  YES");
  mvprintw(13, F_X_START + WIDTH * CELL_SIZE + 3, "  q    -  NO");
}

/**
 * @brief Инициализация цветов и цветовых пар
 */
void init_colors() {
  start_color();
  init_color(COLOR_ORANGE, 800, 429, 0);
  init_color(COLOR_CUSTOM_YELLOW, 900, 900, 0);
  init_color(COLOR_VIOLET, 442, 100, 800);
  init_color(COLOR_CUSTOM_MAGENTA, 1000, 200, 1000);
  init_pair(MAGENTA_P, COLOR_CUSTOM_MAGENTA, COLOR_BLACK);
  init_pair(RED_P, COLOR_RED, COLOR_BLACK);
  init_pair(ORANGE_P, COLOR_ORANGE, COLOR_BLACK);
  init_pair(YELLOW_P, COLOR_CUSTOM_YELLOW, COLOR_BLACK);
  init_pair(GREEN_P, COLOR_GREEN, COLOR_BLACK);
  init_pair(BLUE_P, COLOR_BLUE, COLOR_BLACK);
  init_pair(VIOLET_P, COLOR_VIOLET, COLOR_BLACK);
}

/** @} */  // Конец группы frontend_module