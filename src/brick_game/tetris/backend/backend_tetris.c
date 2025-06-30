#include "backend_tetris.h"

#include "figures.h"

/**
 * @brief Возвращает указатель на текущее состояние игры
 * @return Указатель на структуру GameInfo_t
 * @note Использует статическую переменную для хранения состояния
 */
GameInfo_t *updateCurrentState() {
  static GameInfo_t game = {0};
  return &game;
}

/**
 * @brief Инициализирует начальное состояние игры
 * @param game Указатель на структуру состояния игры
 * @details Очищает игровое поле, сбрасывает счет и уровень,
 *          загружает рекорд и устанавливает начальное состояние
 */
void game_init(GameInfo_t *game) {
  reset_field();
  generate_figure(&game->next);
  game->score = 0;
  game->high_score = load_max_score();
  game->level = LEVEL_MIN;
  game->speed = SPEED_MIN;
  game->pause = 0;
  game->timer = get_current_time();
  game->state = START;
}

/**
 * @brief Очищает игровое поле
 * @details Устанавливает все ячейки поля в 0
 */
void reset_field() {
  GameInfo_t *game = updateCurrentState();
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      game->field[i][j] = 0;
    }
  }
}

/**
 * @brief Получает текущее время в миллисекундах
 * @return Текущее время в миллисекундах
 */
long long int get_current_time() {
  struct timeval now;
  gettimeofday(&now, NULL);
  return (long long)now.tv_sec * 1000 + now.tv_usec / 1000;
}

/**
 * @brief Преобразует ввод пользователя в игровое действие
 * @param user_input Код нажатой клавиши
 * @return Соответствующее действие UserAction_t
 */
UserAction_t get_action(int user_input) {
  UserAction_t res = -1;
  switch (user_input) {
    case KEY_DOWN:
      res = Down;
      break;
    case KEY_LEFT:
      res = Left;
      break;
    case KEY_RIGHT:
      res = Right;
      break;
    case SPACE_KEY:
      res = Action;
      break;
    case PAUSE_KEY:
      res = Pause;
      break;
    case ESCAPE_KEY:
      res = Terminate;
      break;
    case ENTER_KEY:
      res = Start;
      break;
  }
  return res;
}

/**
 * @brief Обрабатывает пользовательский ввод в зависимости от состояния игры
 * @param action Действие пользователя
 * @param hold Флаг удержания клавиши (не используется)
 */
void userInput(UserAction_t action, bool hold) {
  GameInfo_t *game = updateCurrentState();

  switch (game->state) {
    case START:
      start_state_actions(game, action);
      break;
    case SPAWN:
      spawn_state_actions(game);
      break;
    case MOVING:
      moving_state_actions(game, action);
      break;
    case SHIFTING:
      shifting_state_actions(game);
      break;
    case ATTACHING:
      attaching_state_actions(game);
      break;
    case GAMEOVER:
      gameover_state_actions(game, action);
      break;
    case PAUSE:
      pause_state_actions(game, action);
      break;
    default:
      break;
  }
  (void)hold;
}

/**
 * @brief Обработка состояния START
 * @param game Указатель на состояние игры
 * @param action Действие пользователя
 */
void start_state_actions(GameInfo_t *game, UserAction_t action) {
  switch (action) {
    case Start:
      game->state = SPAWN;
      break;
    case Terminate:
      game->state = EXIT_STATE;
      break;
    default:
      game->state = START;
      break;
  }
}

/**
 * @brief Обработка состояния SPAWN
 * @param game Указатель на состояние игры
 * @details Спавнит новую фигуру и проверяет условия завершения игры
 */
void spawn_state_actions(GameInfo_t *game) {
  spawn_figure();
  if (check_figure_overlay()) {
    while (check_figure_overlay()) {
      game->current.y--;
    }
    game->state = GAMEOVER;
  } else
    game->state = MOVING;
}

/**
 * @brief Обработка состояния MOVING (фигура в движении)
 * @param game Указатель на структуру состояния игры
 * @param action Действие пользователя
 */
void moving_state_actions(GameInfo_t *game, UserAction_t action) {
  switch (action) {
    case Left:
      move_left();
      break;
    case Right:
      move_right();
      break;
    case Down:
      while (((collision() & 0b100) != 4)) {
        move_down();
      }
      break;
    case Action:
      rotate_figure();
      break;
    case Terminate:
      game->state = EXIT_STATE;
      break;
    case Pause:
      game->state = PAUSE;
      game->pause = 1;
      break;
    default:
      break;
  }
  if ((get_current_time() - game->timer) >= game->speed) {
    game->timer = get_current_time();
    game->state = SHIFTING;
  }
}

/**
 * @brief Обработка состояния SHIFTING (автоматическое смещение вниз)
 * @param game Указатель на структуру состояния игры
 * @details Перемещает фигуру на 1 клетку вниз и проверяет столкновение:
 * - При столкновении переходит в ATTACHING
 * - Иначе возвращается в MOVING
 */
void shifting_state_actions(GameInfo_t *game) {
  move_down();
  game->state = ((collision() & 0b100) == 4) ? ATTACHING : MOVING;
}

/**
 * @brief Обработка состояния ATTACHING (фиксация фигуры)
 * @param game Указатель на структуру состояния игры
 * @details Выполняет:
 * 1. Фиксацию фигуры на поле
 * 2. Подсчет очков за заполненные линии
 * 3. Обновление уровня сложности
 * 4. Переход в состояние SPAWN для новой фигуры
 */
void attaching_state_actions(GameInfo_t *game) {
  attached_figure();
  calculate_score();
  update_level();
  game->state = SPAWN;
}

/**
 * @brief Обработка состояния PAUSE (пауза)
 * @param game Указатель на структуру состояния игры
 * @param action Действие пользователя
 * @details Обрабатывает:
 * - Снятие паузы (возврат в MOVING)
 * - Выход из игры
 */
void pause_state_actions(GameInfo_t *game, UserAction_t action) {
  switch (action) {
    case Pause:
      game->pause = 0;
      game->state = MOVING;
      game->timer = get_current_time();
      break;
    case Terminate:
      game->state = EXIT_STATE;
      break;
    default:
      break;
  }
}

/**
 * @brief Обработка состояния GAMEOVER (конец игры)
 * @param game Указатель на структуру состояния игры
 * @param action Действие пользователя
 * @details Обрабатывает:
 * - Restart игры (по нажатию Start)
 * - Выход из игры (по нажатию Terminate)
 */
void gameover_state_actions(GameInfo_t *game, UserAction_t action) {
  switch (action) {
    case Start:
      reset_figure(&game->next);
      game_init(game);
      game->state = SPAWN;
      break;
    case Terminate:
      game->state = EXIT_STATE;
      break;
    default:
      break;
  }
}

/**
 * @brief Проверяет столкновения фигуры
 * @return Битовая маска столкновений (0b100 - низ, 0b010 - лево, 0b001 - право)
 */
int collision() {
  GameInfo_t *game = updateCurrentState();
  int collision = 0;
  int x = game->current.x;
  int y = game->current.y;  ///< координаты текущей фигуры
  for (int i = 0; i < 4; i++, y++) {
    for (int j = 0; j < 4; j++, x++) {
      if (game->current.view[i][j] != 0 &&
          (game->field[y + 1][x] != 0 || y == HEIGHT - 1))
        collision |= (1 << 2);  ///< столкновение снизу (0b100(4))
      if (game->current.view[i][j] != 0 && game->field[y][x - 1] != 0)
        collision |= (1 << 1);  ///< столкновение слева (0b010(2))
      if (game->current.view[i][j] != 0 && game->field[y][x + 1] != 0)
        collision |= 1;  ///< столкновение справа (0b001(1))
    }
    x = game->current.x;
  }
  return collision;
}

/**
 * @brief Проверяет наложение текущей фигуры на занятые клетки поля
 * @return 1 если есть наложение, 0 если наложения нет
 * @details Функция проверяет все 4x4 клетки текущей фигуры на пересечение
 *          с непустыми клетками игрового поля. Проверка выполняется для
 *          текущей позиции фигуры (game->current.x, game->current.y).
 * @note Использует битовое представление фигуры (game->current.view),
 *       где 0 - пустая клетка, не 0 - часть фигуры.
 */
int check_figure_overlay() {
  GameInfo_t *game = updateCurrentState();
  int overlay = 0;
  int x = game->current.x;
  int y = game->current.y;
  for (int i = 0; i < 4; i++, y++) {
    for (int j = 0; j < 4; j++, x++) {
      if (game->current.view[i][j] != 0 && game->field[y][x] != 0) overlay = 1;
    }
    x = game->current.x;
  }
  return overlay;
}

/**
 * @brief Проверяет выход фигуры за границы игрового поля
 * @return Код ошибки:
 *         - 0: нет выхода за границы
 *         - 1: выход за левую границу
 *         - 2: выход за правую границу
 *         - 3: выход за нижнюю границу
 * @note Проверяет все 4x4 клетки текущей фигуры
 */
int check_leaving_field() {
  GameInfo_t *game = updateCurrentState();
  int leave = 0;
  int x = game->current.x;
  int y = game->current.y;
  for (int i = 0; i < 4; i++, y++) {
    for (int j = 0; j < 4; j++, x++) {
      if (game->current.view[i][j] != 0 && (x < 0))
        leave = 1;
      else if (game->current.view[i][j] != 0 && (x > WIDTH - 1))
        leave = 2;
      else if (game->current.view[i][j] != 0 && (y > HEIGHT - 1))
        leave = 3;
    }
    x = game->current.x;
  }
  return leave;
}

/**
 * @brief Удаляет заполненные строки и подсчитывает их количество
 * @param[out] lines Указатель для сохранения количества удаленных строк
 * @return 1 если были удалены строки, 0 если нет
 */
int remove_full_lines(int *lines) {
  const GameInfo_t *game = updateCurrentState();
  int removed = 0;
  for (int i = HEIGHT - 1; i >= 0; i--) {
    int full = 1;
    for (int j = 0; j < WIDTH; j++) {
      if (game->field[i][j] == 0) full = 0;
    }
    if (full) {
      drop_lines(i);
      *lines += 1;
      removed = 1;
    }
  }
  return removed;
}

/**
 * @brief Сдвигает строки игрового поля вниз начиная с указанной линии
 * @param line Номер линии, с которой начинается сдвиг
 */
void drop_lines(int line) {
  GameInfo_t *game = updateCurrentState();
  for (int i = line; i > 0; i--) {
    for (int j = 0; j < WIDTH; j++) {
      game->field[i][j] = game->field[i - 1][j];
    }
  }
}

/**
 * @brief Подсчитывает и обновляет счет игрока
 * @details Удаляет заполненные линии и начисляет очки
 * Обновляет рекорд, если текущий счет его превышает
 */
void calculate_score() {
  GameInfo_t *game = updateCurrentState();
  int lines = 0;
  while (remove_full_lines(&lines));
  switch (lines) {
    case 1:
      game->score += 100;
      break;
    case 2:
      game->score += 300;
      break;
    case 3:
      game->score += 700;
      break;
    case 4:
      game->score += 1500;
      break;
  }
  if (game->score > game->high_score) {
    game->high_score = game->score;
    save_max_score(game->high_score);
  }
}

/**
 * @brief Обновляет уровень сложности игры
 * @details Уровень повышается каждые 600 очков:
 *          - Увеличивает скорость игры (уменьшает game->speed)
 *          - Максимальный уровень ограничен LEVEL_MAX
 */
void update_level() {
  GameInfo_t *game = updateCurrentState();
  game->level = (game->score / 600) + 1;
  if (game->level > LEVEL_MAX) game->level = LEVEL_MAX;
  game->speed = SPEED_MIN - (game->level * 80);
}

/**
 * @brief Сохраняет рекорд в файл
 * @param high_score Значение рекорда для сохранения
 * @note Файл сохраняется в build/high_score.txt
 */
void save_max_score(int high_score) {
  FILE *file = fopen("build/high_score.txt", "w");
  if (file != NULL) {
    fprintf(file, "%d", high_score);
    fclose(file);
  } else {
    perror("Error saving high score");
  }
}

/**
 * @brief Загружает рекорд из файла
 * @return Загруженное значение рекорда или 0 если файл не существует
 */
int load_max_score() {
  int high_score = 0;
  FILE *file = fopen("build/high_score.txt", "r");
  if (file != NULL) {
    fscanf(file, "%d", &high_score);
    fclose(file);
  }
  return high_score;
}
