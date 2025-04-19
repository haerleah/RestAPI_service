/**
 * @file s21_controller.cpp
 * @brief Controller source code.
 */
#include "s21_controller.h"

extern "C" {

namespace s21 {

GameStatus_t getGameStatus() {
  return SnakeFacade::Instance().getCurrentGameStatus();
}

GameInfo_t updateScene() { return updateCurrentState(); }

void processUserAction(UserAction_t action, bool hold) {
  userInput(action, hold);
}

void initializeGame() { SnakeFacade::Instance().initializeGame(); }

void freeGameInfo(GameInfo_t gameInfo) { freeGameState(gameInfo); }

}  // namespace s21
}
