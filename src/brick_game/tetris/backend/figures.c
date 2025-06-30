#include "figures.h"

#include <stdlib.h>
#include <time.h>

#include "backend_tetris.h"

/**
 * @brief Сбрасывает фигуру в нулевое состояние
 * @param figure Указатель на структуру тетромино
 * @details Заполняет матрицу 4x4 нулями (пустая фигура)
 */
void reset_figure(Tetramino *figure) {
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++) figure->view[i][j] = 0;
}

/**
 * @brief Генерирует случайную фигуру тетромино
 * @param figure Указатель на структуру для заполнения
 */
void generate_figure(Tetramino *figure) {
  int number = rand() % 7;
  figure->rows = 3;
  figure->cols = 3;
  if (number == 0) {
    figure->rows = 2;
    figure->cols = 4;
    for (int i = 0; i < 4; i++) figure->view[1][i] = COLOR_RED;
    figure->type = 'I';
  }
  if (number == 1) {
    figure->rows = 2;
    figure->cols = 2;
    for (int i = 1; i < 3; i++) figure->view[0][i] = COLOR_CUSTOM_MAGENTA;
    for (int i = 1; i < 3; i++) figure->view[1][i] = COLOR_CUSTOM_MAGENTA;
    figure->type = 'O';
  }
  if (number == 2) {
    figure->view[0][2] = COLOR_CUSTOM_YELLOW;
    for (int j = 0; j < 3; j++) figure->view[1][j] = COLOR_CUSTOM_YELLOW;
    figure->type = 'L';
  }
  if (number == 3) {
    figure->view[0][0] = COLOR_ORANGE;
    for (int j = 0; j < 3; j++) figure->view[1][j] = COLOR_ORANGE;
    figure->type = 'J';
  }
  if (number == 4) {
    for (int i = 1; i < 3; i++) figure->view[0][i] = COLOR_GREEN;
    for (int i = 0; i < 2; i++) figure->view[1][i] = COLOR_GREEN;
    figure->type = 'S';
  }
  if (number == 5) {
    figure->view[0][1] = COLOR_BLUE;
    for (int i = 0; i < 3; i++) figure->view[1][i] = COLOR_BLUE;
    figure->type = 'T';
  }
  if (number == 6) {
    for (int i = 0; i < 2; i++) figure->view[0][i] = COLOR_VIOLET;
    for (int i = 1; i < 3; i++) figure->view[1][i] = COLOR_VIOLET;
    figure->type = 'Z';
  }
}

/**
 * @brief Поворачивает текущую фигуру на 90 градусов
 * @details Особые случаи:
 *          - O-фигура не поворачивается
 *          - I-фигура требует особой обработки
 *          - Корректирует позицию при выходе за границы
 */
void rotate_figure() {
  GameInfo_t *game = updateCurrentState();
  int temp_view[4][4] = {0};
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      temp_view[i][j] = game->current.view[i][j];
    }
  }
  if (game->current.type != 'O' && game->current.type != 'I') {
    for (int i = 0; i < game->current.rows; i++) {
      for (int j = 0; j < game->current.cols; j++)
        game->current.view[i][j] = temp_view[game->current.cols - 1 - j][i];
    }
  }
  if (game->current.type == 'I' && game->current.y >= 0) {
    for (int i = 0; i < 4; i++) {
      int temp = game->current.view[1][i];
      game->current.view[1][i] = game->current.view[i][1];
      game->current.view[i][1] = temp;
    }
    int cols_temp = game->current.cols;
    game->current.cols = game->current.rows;
    game->current.rows = cols_temp;
  }

  if (check_leaving_field() == 1)
    if ((collision() & 0b001) != 1) game->current.x++;
  if (check_leaving_field() == 2) {
    if ((collision() & 0b010) != 2) game->current.x--;
    if (game->current.type == 'I')
      if ((collision() & 0b010) != 2) game->current.x--;
  }

  if (check_figure_overlay() || check_leaving_field()) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) game->current.view[i][j] = temp_view[i][j];
    }
  }
}

/**
 * @brief Появление новой фигурки на поле
 * @details Берет следующую фигуру (game->next), помещает ее в текущую
 * (game->current) и генерирует новую следующую фигуру
 */
void spawn_figure() {
  GameInfo_t *game = updateCurrentState();
  reset_figure(&game->current);
  game->current = game->next;

  game->current.x = WIDTH / 2 - 2;
  game->current.y = game->current.type == 'I' ? -1 : 0;

  reset_figure(&game->next);
  generate_figure(&game->next);
}

/**
 * @brief Двигает фигуру влево с проверкой коллизий
 */
void move_left() {
  GameInfo_t *game = updateCurrentState();
  if ((collision() & 0b010) != 2) game->current.x--;
  if (check_leaving_field()) game->current.x++;
}

/**
 * @brief Двигает фигуру вправо с проверкой коллизий
 */
void move_right() {
  GameInfo_t *game = updateCurrentState();
  if ((collision() & 0b001) != 1) game->current.x++;
  if (check_leaving_field()) game->current.x--;
}

/**
 * @brief Двигает фигуру вниз с проверкой коллизий
 */
void move_down() {
  GameInfo_t *game = updateCurrentState();
  if (!check_leaving_field() && (collision() & 0b100) != 4) game->current.y++;
}

/**
 * @brief Прикрепление фигуры к игровому полю
 * @details Переносит все непустые клетки фигуры в игровое поле
 */
void attached_figure() {
  GameInfo_t *game = updateCurrentState();
  int x = game->current.x;
  int y = game->current.y;  // текущие координаты фигуры (левый верхний угол 4×4
                            // матрицы фигуры)
  for (int i = 0; i < 4; i++, y++) {
    for (int j = 0; j < 4; j++, x++) {
      if (game->current.view[i][j] != 0)
        game->field[y][x] = game->current.view[i][j];
    }  // значение ячейки фигуры копируется на соответствующую позицию поля
    x = game->current.x;
  }
}
