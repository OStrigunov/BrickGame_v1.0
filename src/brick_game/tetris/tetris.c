/**
 * @file main.c
 * @brief Главный модуль игры Tetris
 * @defgroup main_module Main Game Module
 * @{
 */

#include "tetris.h"

/**
 * @brief Точка входа в программу
 * @return 0 при успешном завершении
 * @details Инициализирует ncurses, запускает главный игровой цикл
 * и корректно завершает работу с ncurses.
 */
int main(void) {
  init_ncurses();
  main_game_loop();
  endwin();

  return 0;
}

/**
 * @brief Основной игровой цикл
 * @details Управляет:
 * - Отрисовкой игрового состояния
 * - Обработкой пользовательского ввода
 * - Обновлением экрана
 * Работает до перехода игры в состояние EXIT_STATE
 *
 * @note Частота обновления экрана зависит от скорости ввода пользователя
 * @see GameInfo_t, userInput(), print_game_screen()
 */
void main_game_loop() {
  GameInfo_t *game = updateCurrentState();
  game_init(game);  // Инициализация состояния игры
  while (game->state != EXIT_STATE) {
    erase();                   // очистка экрана (ncurses)
    print_game_screen(*game);  // отображение игрового поля, фигур и статистики
    userInput(get_action(getch()), 0);  // обработка пользовательского ввода
    refresh();  // обновление экрана (ncurses)
  }
}

/** @} */  // Конец группы main_module