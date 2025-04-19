/**
 * @file s21_tetris_back.c
 * @brief Game logic source code
 */

#include "s21_tetris.h"
#include "tetfig.h"

/* -------------------------------------------------------------------------- */
/*                      SINGLETON-LIKE GETTERS & STATICS                      */
/* -------------------------------------------------------------------------- */

int*** get_sub_field(void) {
  static int** subFieldPtr;
  return &subFieldPtr;
}

int*** get_coords(void) {
  static int** figureCoordsPtr;
  return &figureCoordsPtr;
}

GameInfo_t* get_info_instance(void) {
  static GameInfo_t tetrisGame;
  return &tetrisGame;
}

GameStatus_t* get_status_instance(void) {
  static GameStatus_t gameStatus;
  return &gameStatus;
}

ThreadStruct* get_thread_struct_instance() {
  static ThreadStruct timerThreadStruct;
  return &timerThreadStruct;
}

ThreadStruct* get_game_thread_struct_instance() {
  static ThreadStruct gameThreadStruct;
  return &gameThreadStruct;
}

int get_figure_index(int* index) {
  static int figIndex;
  if (index != NULL) {
    figIndex = *index;
  }
  return figIndex;
}

float* get_timer(void) {
  static float gameTimer;
  return &gameTimer;
}

pthread_t* get_main_thread() {
  static pthread_t mainThread;
  return &mainThread;
}

bool* get_signal_handled_flag() {
  static bool signalHandled;
  return &signalHandled;
}

int* get_first_plant_flag() {
  static int firstPlantFlag;
  return &firstPlantFlag;
}

UserAction_t* get_action_instance() {
  static UserAction_t action;
  return &action;
}

bool* get_hold_instance() {
  static bool hold;
  return &hold;
}

/* -------------------------------------------------------------------------- */
/*                           SIGNAL HANDLER & FREE                            */
/* -------------------------------------------------------------------------- */

/**
 * @brief Signal handler for SIGUSR1
 */
void sig_handler(int signo) {
  if (signo == SIGUSR1) {
    bool* threadsStoppedFlag = get_signal_handled_flag();
    if (*threadsStoppedFlag == false) {
      *threadsStoppedFlag = true;
      memfree(get_info_instance());
    }
  }
}

void memfree(GameInfo_t* tetrisGame) {
  /* Cancel timer thread */
  ThreadStruct* timerThread = get_thread_struct_instance();
  pthread_cancel(timerThread->thread);
  pthread_join(timerThread->thread, NULL);
  pthread_mutex_destroy(&timerThread->mutex);

  /* Cancel game thread */
  ThreadStruct* gameThread = get_game_thread_struct_instance();
  pthread_mutex_destroy(&gameThread->mutex);
  pthread_cancel(gameThread->thread);
  pthread_join(gameThread->thread, NULL);

  /* Free fields */
  int*** subFieldPtr = get_sub_field();
  int** coordsPtr = *get_coords();

  for (int i = 0; i < ROWS_FIELD; ++i) {
    free(tetrisGame->field[i]);
    free((*subFieldPtr)[i]);
  }
  free(tetrisGame->field);
  free(*subFieldPtr);

  for (int i = 0; i < 4; ++i) {
    free(tetrisGame->next[i]);
  }
  free(tetrisGame->next);

  for (int i = 0; i < 2; ++i) {
    free(coordsPtr[i]);
  }
  free(coordsPtr);
}

/* -------------------------------------------------------------------------- */
/*                            THREAD FUNCTIONS                                */
/* -------------------------------------------------------------------------- */

void* timer_handler(void* arg) {
  float* gameTimer = get_timer();
  ThreadStruct* timerThread = get_thread_struct_instance();
  GameInfo_t* gameInfo = (GameInfo_t*)arg;

  while (*get_status_instance() != EXIT) {
    pthread_mutex_lock(&timerThread->mutex);
    if (gameInfo->speed == 1) {
      *gameTimer += 150 * 1e-3f;
    } else {
      *gameTimer += (75 * gameInfo->speed * 1e-3f);
    }
    pthread_mutex_unlock(&timerThread->mutex);
    usleep(5 * 10e3);
  }
  return NULL;
}

/* -------------------------------------------------------------------------- */
/*                            INITIALIZATION                                  */
/* -------------------------------------------------------------------------- */

void initialize_game(void) {
  srand(time(NULL));
  GameInfo_t* tetrisGame = get_info_instance();
  GameStatus_t* gameStatus = get_status_instance();
  *gameStatus = START;

  UserAction_t* currentAction = get_action_instance();
  *currentAction = Up;

  int*** subFieldPtr = get_sub_field();
  tetrisGame->score = 0;
  tetrisGame->speed = 1;
  tetrisGame->pause = 0;
  tetrisGame->level = 1;
  *get_first_plant_flag() = 0;

  bool* threadsStoppedFlag = get_signal_handled_flag();
  *threadsStoppedFlag = false;

  /* Allocate memory for field and sub-field */
  tetrisGame->field = (int**)calloc(ROWS_FIELD, sizeof(int*));
  *subFieldPtr = (int**)calloc(ROWS_FIELD, sizeof(int*));
  for (int i = 0; i < ROWS_FIELD; ++i) {
    tetrisGame->field[i] = (int*)calloc(COLS_FIELD, sizeof(int));
    (*subFieldPtr)[i] = (int*)calloc(COLS_FIELD, sizeof(int));
  }

  /* Allocate memory for next figure */
  tetrisGame->next = (int**)calloc(4, sizeof(int*));
  for (int i = 0; i < 4; ++i) {
    tetrisGame->next[i] = (int*)calloc(4, sizeof(int));
  }

  /* Allocate memory for coordinates */
  int*** coordsPtr = get_coords();
  (*coordsPtr) = (int**)calloc(2, sizeof(int*));
  for (int i = 0; i < 2; ++i) {
    (*coordsPtr)[i] = (int*)calloc(4, sizeof(int));
  }

  /* Load high score from file */
  FILE* database = fopen("tetris_score", "a+");
  fscanf(database, "%d", &tetrisGame->high_score);
  fclose(database);

  /* Store current thread as the main thread */
  *get_main_thread() = pthread_self();

  /* Set up signal handler for SIGUSR1 */
  struct sigaction sa;
  sa.sa_handler = sig_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGUSR1, &sa, NULL);

  /* Create and init the timer thread */
  ThreadStruct* timerThread = get_thread_struct_instance();
  pthread_create(&timerThread->thread, NULL, timer_handler,
                 get_info_instance());
  pthread_mutex_init(&timerThread->mutex, NULL);

  /* Create and init the game thread */
  ThreadStruct* gameThread = get_game_thread_struct_instance();
  pthread_create(&gameThread->thread, NULL, game_handler, NULL);
  pthread_mutex_init(&gameThread->mutex, NULL);
}

/* -------------------------------------------------------------------------- */
/*                            GAME STATE UPDATE                               */
/* -------------------------------------------------------------------------- */

GameInfo_t updateCurrentState(void) {
  GameInfo_t* tetrisGame = get_info_instance();
  GameInfo_t copyGameInfo = *tetrisGame;
  GameStatus_t* gameStatus = get_status_instance();
  int figIndex = get_figure_index(NULL);

  /* Allocate new field for a copy */
  copyGameInfo.field = (int**)calloc(ROWS_FIELD, sizeof(int*));
  for (int i = 0; i < ROWS_FIELD; ++i) {
    copyGameInfo.field[i] = (int*)calloc(COLS_FIELD, sizeof(int));
  }

  copyGameInfo.next = (int**)calloc(4, sizeof(int*));
  for (int i = 0; i < 4; ++i) {
    copyGameInfo.next[i] = (int*)calloc(4, sizeof(int));
  }

  if (*gameStatus != START) {
    for (int i = 0; i < ROWS_FIELD; ++i) {
      for (int j = 0; j < COLS_FIELD; ++j) {
        copyGameInfo.field[i][j] = tetrisGame->field[i][j];
      }
    }

    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; ++j) {
        copyGameInfo.next[i][j] = tetrisGame->next[i][j];
      }
    }

    if (*gameStatus != SPAWN) {
      unit_fields(&copyGameInfo, figIndex);
    }
  }
  return copyGameInfo;
}

/* -------------------------------------------------------------------------- */
/*                           GAME THREAD (FSM)                                */
/* -------------------------------------------------------------------------- */

void* game_handler(void* arg) {
  if (arg == NULL) {
    /* Заглушка, чтобы не ругался компилятор */
    printf(" ");
  }
  bool continueLoop = true;
  static int figureIndex;
  static bool attachFlag = false;

  while (continueLoop) {
    ThreadStruct* gameThread = get_game_thread_struct_instance();
    pthread_mutex_lock(&gameThread->mutex);

    GameStatus_t* gameStatus = get_status_instance();
    UserAction_t* action = get_action_instance();
    GameInfo_t* tetrisGame = get_info_instance();
    float* timerVal = get_timer();

    switch (*gameStatus) {
      case START:
        switch (*action) {
          case Start:
            *gameStatus = SPAWN;
            break;
          case Terminate:
            *gameStatus = EXIT;
            break;
          default:
            *gameStatus = START;
            break;
        }
        break;

      case SPAWN:
        init_block(tetrisGame, &figureIndex);
        *gameStatus = MOVING;
        break;

      case MOVING:
        if (*action == Left) move_left(tetrisGame);
        if (*action == Right) move_right(tetrisGame);
        if (*action == Action && !attachFlag) {
          rotate_block(tetrisGame, figureIndex);
        }
        if (*action == Down) {
          force_down(tetrisGame);
        }
        if (*action == Terminate) {
          *gameStatus = EXIT;
        }
        if (*action == Pause) {
          pause_game(tetrisGame);
          *gameStatus = PAUSE;
        }
        if (*gameStatus != EXIT && *gameStatus != PAUSE) {
          if ((check_horizontal_collide(tetrisGame) && *timerVal > 1.5f) ||
              *action == Down) {
            *gameStatus = ATTACHING;
            break;
          }
        }
        __attribute__((fallthrough));

      case SHIFTING:
        if (*timerVal > 1.5f) {
          pthread_mutex_lock(&get_thread_struct_instance()->mutex);
          move_down(tetrisGame);
          if (check_horizontal_collide(tetrisGame)) {
            if (attachFlag) {
              *gameStatus = ATTACHING;
            } else {
              *gameStatus = MOVING;
              attachFlag = true;
            }
          }
          *timerVal = 0;
          pthread_mutex_unlock(&get_thread_struct_instance()->mutex);
        }
        break;

      case ATTACHING:
        unit_fields(tetrisGame, figureIndex);
        line_handler(tetrisGame);
        score_handler(tetrisGame);
        if (check_gameover(tetrisGame)) {
          *gameStatus = GAMEOVER;
        } else {
          *gameStatus = SPAWN;
        }
        attachFlag = false;
        break;

      case GAMEOVER:
        if (*action == Start) {
          *gameStatus = SPAWN;
          free_field(tetrisGame);
        } else if (*action == Terminate) {
          *gameStatus = EXIT;
        }
        break;

      case EXIT:
        continueLoop = false;
        pthread_kill(*get_main_thread(), SIGUSR1);
        break;

      case PAUSE:
        if (*action == Pause) {
          pause_game(tetrisGame);
        }
        if (tetrisGame->pause == 0) {
          *gameStatus = MOVING;
        }
        break;
    }

    *action = Up;
    pthread_mutex_unlock(&gameThread->mutex);
    usleep(5 * 10e3);
  }
  return NULL;
}

/* -------------------------------------------------------------------------- */
/*                       USER INPUT & SIMPLE FUNCTIONS                        */
/* -------------------------------------------------------------------------- */

void userInput(UserAction_t action, bool hold) {
  ThreadStruct* gameThread = get_game_thread_struct_instance();
  pthread_mutex_lock(&gameThread->mutex);

  UserAction_t* currentAction = get_action_instance();
  *currentAction = action;

  bool* currentHold = get_hold_instance();
  *currentHold = hold;

  pthread_mutex_unlock(&gameThread->mutex);
}

void pause_game(GameInfo_t* tetrisGame) {
  tetrisGame->pause = !tetrisGame->pause;
}

/* -------------------------------------------------------------------------- */
/*                      BLOCK SPAWNING & RENDERING                            */
/* -------------------------------------------------------------------------- */

void init_block(GameInfo_t* tetrisGame, int* figureIndex) {
  int isFirstPlant = *get_first_plant_flag();
  int nextRandomIndex = 0;

  for (int i = 0; i < 7; ++i) {
    if (is_equal(tetrisGame->next, tet_fig[i])) {
      nextRandomIndex = i;
    }
  }

  int currentRandomIndex = 0;
  if (!isFirstPlant) {
    currentRandomIndex = rand() % 7;
  } else {
    currentRandomIndex = nextRandomIndex;
  }
  nextRandomIndex = rand() % 7;

  plant_figure(tetrisGame, currentRandomIndex);

  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      tetrisGame->next[i][j] = tet_fig[nextRandomIndex][i][j];
    }
  }

  render_field();
  *figureIndex = currentRandomIndex + 1;
  get_figure_index(figureIndex);
}

void plant_figure(GameInfo_t* tetrisGame, int currentRandomIndex) {
  int** coords = *get_coords();
  int firstPlantFlag = *get_first_plant_flag();

  if (!firstPlantFlag) {
    int coordI = 0;
    int coordJ = -1;
    int iIndex = 0;
    for (int i = -3; i <= 0; ++i) {
      int jIndex = 0;
      for (int j = 3; j <= 6; ++j) {
        if (tet_fig[currentRandomIndex][iIndex][jIndex]) {
          coordJ++;
          coords[coordI++][coordJ] = i;
          coords[coordI--][coordJ] = j;
        }
        jIndex++;
      }
      iIndex++;
    }
    *get_first_plant_flag() = 1;
  } else {
    int coordI = 0;
    int coordJ = -1;
    int iIndex = 0;
    for (int i = -2; i <= 1; ++i) {
      int jIndex = 0;
      for (int j = 3; j <= 6; ++j) {
        if (tetrisGame->next[iIndex][jIndex]) {
          coordJ++;
          coords[coordI++][coordJ] = i;
          coords[coordI--][coordJ] = j;
        }
        jIndex++;
      }
      iIndex++;
    }
  }
}

void render_field(void) {
  int** coords = *get_coords();
  int*** subFieldPtr = get_sub_field();

  for (int i = 0; i < ROWS_FIELD; ++i) {
    for (int j = 0; j < COLS_FIELD; ++j) {
      (*subFieldPtr)[i][j] = 0;
    }
  }
  for (int i = 0; i < 4; ++i) {
    if (coords[0][i] >= 0 && coords[0][i] < ROWS_FIELD)
      (*subFieldPtr)[coords[0][i]][coords[1][i]] = 1;
  }
}

/* -------------------------------------------------------------------------- */
/*                           MOVEMENT & COLLISION                             */
/* -------------------------------------------------------------------------- */

void move_down(GameInfo_t* tetrisGame) {
  int** coords = *get_coords();
  for (int i = 0; i < 4; ++i) {
    coords[0][i]++;
    if (coords[0][i] >= 0 && coords[0][i] < ROWS_FIELD) {
      if (coords[0][i] == ROWS_FIELD ||
          tetrisGame->field[coords[0][i]][coords[1][i]] != 0) {
        for (int j = i; j >= 0; --j) {
          coords[0][j]--;
        }
        break;
      }
    }
  }
  render_field();
}

void move_left(GameInfo_t* tetrisGame) {
  int** coords = *get_coords();
  int tempCoord[2][4];
  for (int i = 0; i < 4; ++i) {
    tempCoord[0][i] = coords[0][i];
    tempCoord[1][i] = coords[1][i];
  }

  for (int i = 0; i < 4; ++i) {
    coords[1][i]--;
  }

  for (int i = 0; i < 4; ++i) {
    if (coords[0][i] >= 0 && coords[0][i] < ROWS_FIELD) {
      if (check_vertical_collide(tetrisGame) ||
          tetrisGame->field[coords[0][i]][coords[1][i]] != 0) {
        for (int k = 0; k < 4; ++k) {
          coords[0][k] = tempCoord[0][k];
          coords[1][k] = tempCoord[1][k];
        }
        break;
      }
    }
  }
  render_field();
}

void move_right(GameInfo_t* tetrisGame) {
  int** coords = *get_coords();
  int tempCoord[2][4];
  for (int i = 0; i < 4; ++i) {
    tempCoord[0][i] = coords[0][i];
    tempCoord[1][i] = coords[1][i];
  }

  for (int i = 0; i < 4; ++i) {
    coords[1][i]++;
  }

  if (check_vertical_collide(tetrisGame)) {
    for (int i = 0; i < 4; ++i) {
      coords[0][i] = tempCoord[0][i];
      coords[1][i] = tempCoord[1][i];
    }
  }
  render_field();
}

void force_down(GameInfo_t* tetrisGame) {
  while (!check_horizontal_collide(tetrisGame)) {
    move_down(tetrisGame);
  }
  render_field();
}

/* -------------------------------------------------------------------------- */
/*                           COLLISION CHECKS                                 */
/* -------------------------------------------------------------------------- */

bool check_horizontal_collide(GameInfo_t* tetrisGame) {
  bool result = false;
  int** coords = *get_coords();

  for (int i = 0; i < 4; ++i) {
    if (coords[0][i] == ROWS_FIELD - 1) {
      result = true;
      break;
    }
  }

  int uniqueX[4] = {21, 21, 21, 21};
  int minY[4] = {21, 21, 21, 21};
  int k = 0;
  for (int i = 0; i < 4; ++i) {
    if (coords[0][i] >= 0 && coords[0][i] < ROWS_FIELD) {
      int flag = 0;
      for (int j = 0; j < 4; ++j) {
        if (coords[1][i] == uniqueX[j]) {
          if (minY[j] < coords[0][i]) {
            minY[j] = coords[0][i];
          }
          flag = 1;
          break;
        }
      }
      if (!flag) {
        uniqueX[k] = coords[1][i];
        minY[k++] = coords[0][i];
      }
    }
  }

  for (int i = 0; i < 4; ++i) {
    if ((minY[i] < 19 && uniqueX[i] < 19)) {
      if (tetrisGame->field[minY[i] + 1][uniqueX[i]] != 0) {
        result = true;
      } else if (minY[i] == 19) {
        result = true;
      }
    }
  }
  return result;
}

bool check_vertical_collide(GameInfo_t* tetrisGame) {
  bool result = false;
  int** coords = *get_coords();
  for (int i = 0; i < 4; ++i) {
    if (coords[1][i] == -1 || coords[1][i] == COLS_FIELD) {
      result = true;
      break;
    }
  }

  int maxX[4] = {-1, -1, -1, -1};
  int minX[4] = {21, 21, 21, 21};
  int maxY[4] = {21, 21, 21, 21};
  int minY[4] = {21, 21, 21, 21};
  int k = 0;
  int _k = 0;

  for (int i = 0; i < 4; ++i) {
    if (coords[0][i] >= 0 && coords[0][i] < ROWS_FIELD) {
      int minFlag = 0;
      int maxFlag = 0;
      for (int j = 3; j >= 0; --j) {
        if (coords[1][i] >= minX[j] && coords[0][i] == minY[j]) {
          minFlag = 1;
          break;
        }
        if (coords[1][i] < minX[j] && coords[0][i] == minY[j]) {
          k--;
          break;
        }
      }
      for (int j = 0; j < 4; ++j) {
        if (coords[1][i] <= maxX[j] && coords[0][i] == maxY[j]) {
          maxFlag = 1;
          break;
        }
        if (coords[1][i] > maxX[j] && coords[0][i] == maxY[j]) {
          _k--;
          break;
        }
      }
      if (!minFlag) {
        minX[k] = coords[1][i];
        minY[k++] = coords[0][i];
      }
      if (!maxFlag) {
        maxX[_k] = coords[1][i];
        maxY[_k++] = coords[0][i];
      }
    }
  }

  if (!result) {
    for (int i = 0; i < 4; ++i) {
      if (maxX[i] != -1 && maxY[i] != 21 && minX[i] != 21 && minY[i] != 21) {
        if (tetrisGame->field[maxY[i]][maxX[i]] != 0 ||
            tetrisGame->field[minY[i]][minX[i]] != 0) {
          result = true;
        }
      }
    }
  }
  return result;
}

/* -------------------------------------------------------------------------- */
/*                       FIGURE ROTATION & COMPARISON                         */
/* -------------------------------------------------------------------------- */

void rotate_block(GameInfo_t* tetrisGame, int figureIndex) {
  if (figureIndex != 1) {
    int** coords = *get_coords();
    int pivotX = 0;
    int pivotY = 0;
    int tempCoord[2][4];

    get_center(&pivotX, &pivotY, figureIndex);

    for (int i = 0; i < 4; ++i) {
      tempCoord[0][i] = coords[0][i];
      tempCoord[1][i] = coords[1][i];
      int tmp = coords[1][i];
      coords[1][i] = pivotX + pivotY - coords[0][i];
      coords[0][i] = tmp + pivotY - pivotX;
    }

    for (int i = 0; i < 4; ++i) {
      if (coords[0][i] < 0 || coords[0][i] >= ROWS_FIELD || coords[1][i] < 0 ||
          coords[1][i] >= COLS_FIELD ||
          tetrisGame->field[coords[0][i]][coords[1][i]]) {
        for (int k = 0; k < 4; ++k) {
          coords[0][k] = tempCoord[0][k];
          coords[1][k] = tempCoord[1][k];
        }
        break;
      }
    }
  }
  render_field();
}

void get_center(int* px, int* py, int figureIndex) {
  int** coords = *get_coords();
  if (figureIndex == 1) {
    *py = coords[0][2];
    *px = coords[1][2];
  } else if (figureIndex == 2) {
    *py = coords[0][2];
    *px = coords[1][2];
  } else if (figureIndex == 3) {
    *py = coords[0][2];
    *px = coords[1][2];
  } else if (figureIndex == 4) {
    *py = coords[0][2];
    *px = coords[1][2];
  } else if (figureIndex == 5) {
    *py = coords[0][3];
    *px = coords[1][3];
  } else if (figureIndex == 6) {
    *py = coords[0][2];
    *px = coords[1][2];
  } else if (figureIndex == 7) {
    *py = coords[0][2];
    *px = coords[1][2];
  }
}

bool is_equal(int** figure1, const int (*figure2)[4]) {
  bool result = true;
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      if (figure1[i][j] != figure2[i][j]) {
        result = false;
      }
    }
  }
  return result;
}

/* -------------------------------------------------------------------------- */
/*                         FIELD/UNITED FIELD FUNCTIONS                       */
/* -------------------------------------------------------------------------- */

void unit_fields(GameInfo_t* tetrisGame, int figureIndex) {
  int** coords = *get_coords();
  for (int i = 0; i < 4; ++i) {
    if (coords[0][i] >= 0 && coords[0][i] < ROWS_FIELD)
      tetrisGame->field[coords[0][i]][coords[1][i]] = figureIndex;
  }
}

void free_field(GameInfo_t* tetrisGame) {
  for (int i = 0; i < ROWS_FIELD; ++i) {
    for (int j = 0; j < COLS_FIELD; ++j) {
      tetrisGame->field[i][j] = 0;
    }
  }
  tetrisGame->score = 0;
  tetrisGame->level = 1;
  tetrisGame->speed = 1;
}

/* -------------------------------------------------------------------------- */
/*                          SCORE & LINE HANDLING                             */
/* -------------------------------------------------------------------------- */

void line_handler(GameInfo_t* tetrisGame) {
  int lineCount = 0;
  for (int i = 0; i < ROWS_FIELD; ++i) {
    int isFullLine = 1;
    for (int j = 0; j < COLS_FIELD; ++j) {
      if (tetrisGame->field[i][j] == 0) {
        isFullLine = 0;
        break;
      }
    }
    if (isFullLine) {
      lineCount++;
      destroy_line(tetrisGame, i);
    }
  }

  switch (lineCount) {
    case 1:
      tetrisGame->score += 100;
      break;
    case 2:
      tetrisGame->score += 300;
      break;
    case 3:
      tetrisGame->score += 700;
      break;
    case 4:
      tetrisGame->score += 1500;
      break;
    default:
      break;
  }
}

void score_handler(GameInfo_t* tetrisGame) {
  if (tetrisGame->score > tetrisGame->high_score) {
    tetrisGame->high_score = tetrisGame->score;
    FILE* database = fopen("tetris_score", "w");
    fprintf(database, "%d", tetrisGame->score);
    fclose(database);
  }
  if ((tetrisGame->score / 600) > (tetrisGame->level - 1) &&
      tetrisGame->level < 10) {
    tetrisGame->level += 1;
    if (tetrisGame->level % 2 == 0 && tetrisGame->level > 2) {
      tetrisGame->speed++;
    }
  }
}

void destroy_line(GameInfo_t* tetrisGame, int lineNumber) {
  for (int i = 0; i < COLS_FIELD; ++i) {
    tetrisGame->field[lineNumber][i] = 0;
  }
  for (int i = lineNumber; i > 0; --i) {
    for (int j = 0; j < COLS_FIELD; ++j) {
      if (i != 0) {
        tetrisGame->field[i][j] = tetrisGame->field[i - 1][j];
      } else {
        tetrisGame->field[i][j] = 0;
      }
    }
    int*** subFieldPtr = get_sub_field();
    int** coords = *get_coords();
    for (int k = 0; k < 4; ++k) {
      (*subFieldPtr)[coords[0][k]][coords[1][k]] = 0;
    }
  }
}

bool check_gameover(GameInfo_t* tetrisGame) {
  bool result = false;
  for (int i = 0; i < COLS_FIELD; ++i) {
    if (tetrisGame->field[0][i] != 0) {
      result = true;
      break;
    }
  }
  return result;
}
