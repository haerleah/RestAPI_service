/*
Valgrind сигнализирует об утечке в функции json_tokener_parse(), конкретно - в
используемой внутри newlocale() скорее всего это ошибочный резултат, спустя 15
минут бесконечной работы этой функции виртуальная память занятая процессом, а
также общая использованная память в ОС не изменялись, следовательно происходит
корректная очистка.

Такие результаты Valgrind-a, скорее всего, вызваны мехнизмом отслеживания ссылок
в библиотеке, либо очистка локали лежит в зоне ответственности ОС, и Valgrind-у
не удается полноценно отследить использование памяти процессом. OS:
Ubuntu 24.04.2 LTS
*/

#include <curl/curl.h>
#include <json-c/json.h>
#include <locale.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#ifndef S21_CLIENT_LIBRARY_H
#define S21_CLIENT_LIBRARY_H

typedef enum {
  START,
  SPAWN,
  MOVING,
  SHIFTING,
  ATTACHING,
  GAMEOVER,
  EXIT,
  PAUSE
} GameStatus_t;

typedef enum {
  Start,
  Pause,
  Terminate,
  Left,
  Right,
  Up,
  Down,
  Action
} UserAction_t;

typedef struct {
  int** field;
  int** next;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
} GameInfo_t;

typedef struct {
  char* name;
  int identifier;
} GameType_t;

typedef enum {
  STATUS_OK = 200,
  STATUS_BAD_REQUEST = 400,
  STATUS_NOT_FOUND = 404,
  STATUS_CONFLICT = 409,
  STATUS_INTERNAL_SERVER_ERROR = 500,
  STATUS_OTHER = -1
} ResponseStatus_t;

typedef struct {
  ResponseStatus_t response_status;
  char message[100];
} Response_t;

typedef struct {
  char* memory;
  size_t size;
} MemoryStruct;

void init_library(char* server_url);

void clear_library();

Response_t get_accesible_games(GameType_t** result);

Response_t select_game(GameType_t type);

Response_t submit_action(UserAction_t action, bool hold);

Response_t get_current_state(GameInfo_t* result);

Response_t get_current_game_status(GameStatus_t* result);

#endif