/**
 * @file s21_controller.c
 * @brief Controller source code.
 */
#include "s21_controller.h"

GameStatus_t getGameStatus() { return *get_status_instance(); }

GameInfo_t updateScene() { return updateCurrentState(); }

void processUserAction(UserAction_t action, bool hold) {
  userInput(action, hold);
}

void initializeGame() { initialize_game(); }

void freeGameInfo(GameInfo_t game_info) {
  for (int i = 0; i < ROWS_FIELD; ++i) {
    free(game_info.field[i]);
  }
  free(game_info.field);
  for (int i = 0; i < 4; ++i) {
    free(game_info.next[i]);
  }
  free(game_info.next);
}
