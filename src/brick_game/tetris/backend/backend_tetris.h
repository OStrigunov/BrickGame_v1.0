#ifndef BACKEND_TETRIS_H
#define BACKEND_TETRIS_H

#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

/**
 * @defgroup game_constants Игровые константы
 * @{
 */
/// клавиши игрока
#define ESCAPE_KEY 'q'
#define ENTER_KEY 10
#define SPACE_KEY 32
#define PAUSE_KEY 'p'

/// игровые параметры
#define HEIGHT 20
#define WIDTH 10
#define LEVEL_MAX 10
#define LEVEL_MIN 1
#define SPEED_MIN 900

/// кастомные цвета фигур
#define COLOR_ORANGE 8
#define COLOR_CUSTOM_YELLOW 9
#define COLOR_VIOLET 10
#define COLOR_CUSTOM_MAGENTA 11

/** @} */  // Конец группы game_constants

/**
 * @enum GameState_t
 * @brief Состояния конечного автомата игры
 */
typedef enum {
  START = 0,  ///< Ожидание начала игры
  SPAWN,      ///< Появление новой фигуры
  MOVING,     ///< Движение фигуры
  SHIFTING,   ///< Автоматическое смещение вниз
  ATTACHING,  ///< Фиксация фигуры на поле
  GAMEOVER,   ///< Конец игры
  PAUSE,      ///< Игра на паузе
  EXIT_STATE  ///< Выход из игры
} GameState_t;

/**
 * @struct Tetramino
 * @brief Структура, описывающая фигуру тетриса
 */
typedef struct {
  int view[4][4];  ///< Матрица 4x4, представляющая форму фигуры
  int x, y;   ///< X и Y -координаты фигуры на поле
  char type;  ///< Тип фигуры (I, J, L, O, S, T, Z)
  int rows, cols;  ///< Количество строк и столбцов в фигуре
} Tetramino;

/**
 * @enum UserAction_t
 * @brief Действия пользователя
 */
typedef enum {
  Start,      ///< Начать игру
  Pause,      ///< Поставить на паузу
  Terminate,  ///< Завершить игру
  Left,       ///< Движение влево
  Right,      ///< Движение вправо
  Up,         ///< Движение вверх (не используется)
  Down,       ///< Ускоренное падение
  Action      ///< Поворот фигуры
} UserAction_t;

/**
 * @struct GameInfo_t
 * @brief Полное состояние игры
 */
typedef struct {
  int field[HEIGHT][WIDTH];  ///< Игровое поле
  Tetramino next;            ///< Следующая фигура
  Tetramino current;         ///< Текущая фигура
  int score;                 ///< Текущий счет
  int high_score;            ///< Рекордный счет
  int level;                 ///< Текущий уровень
  int speed;                 ///< Текущая скорость (в мс)
  int pause;                 ///< Флаг паузы (1 - пауза)
  long long timer;  ///< Таймер для автоматического смещения
  GameState_t state;  ///< Текущее состояние игры
} GameInfo_t;

/** @} */  // Конец группы backend_api

/**
 * @defgroup game_functions Игровые функции
 * @brief API для работы с игровой логикой
 * @ingroup backend_api
 * @{
 */

// Основные игровые функции
GameInfo_t *updateCurrentState();
UserAction_t get_action(int user_input);
void userInput(UserAction_t action, bool hold);

// Функции конечного автомата(FSM)
void start_state_actions(GameInfo_t *game, UserAction_t action);
void spawn_state_actions(GameInfo_t *game);
void moving_state_actions(GameInfo_t *game, UserAction_t action);
void shifting_state_actions(GameInfo_t *game);
void attaching_state_actions(GameInfo_t *game);
void pause_state_actions(GameInfo_t *game, UserAction_t action);
void gameover_state_actions(GameInfo_t *game, UserAction_t action);

// Вспомогательные функции
void game_init(GameInfo_t *game);
void reset_field();
long long int get_current_time();

// Функции коллизий
int collision();
int check_figure_overlay();
int check_leaving_field();

// Функции работы с полем
int remove_full_lines(int *lines);
void drop_lines(int line);

// Функции счета и уровней
void calculate_score();
void update_level();
void save_max_score(int high_score);
int load_max_score();

#endif  // BACKEND_TETRIS_H