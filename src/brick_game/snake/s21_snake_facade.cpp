#include "s21_snake_facade.h"

namespace s21 {

/* -------------------------------------------------------------------------- */
/*                         SnakeFacade Implementation                         */
/* -------------------------------------------------------------------------- */

SnakeFacade::SnakeFacade() { currentGame_ = nullptr; }
SnakeFacade::~SnakeFacade() {
  if (currentGame_ != nullptr) {
    delete currentGame_;
  }
}

SnakeFacade& SnakeFacade::Instance() {
  static SnakeFacade facade;
  return facade;
}

void SnakeFacade::initializeGame() {
  currentGame_ = new Game();
  validationFlag_ = true;
}

void SnakeFacade::terminateGame() {
  validationFlag_ = false;
  delete currentGame_;
  currentGame_ = nullptr;
}

void SnakeFacade::invalidateGame() { validationFlag_ = false; }

bool SnakeFacade::isValid() { return validationFlag_; }

std::mutex& SnakeFacade::getMutex() { return facadeMutex_; }

Game& SnakeFacade::getCurrentGame() { return *currentGame_; }

GameStatus_t SnakeFacade::getCurrentGameStatus() {
  std::lock_guard<std::mutex> lock(SnakeFacade::Instance().getMutex());
  if (SnakeFacade::Instance().isValid()) {
    return currentGame_->getStatus();
  } else {
    return EXIT;
  }
}

void userInput(UserAction_t action, bool hold) {
  std::lock_guard<std::mutex> lock(SnakeFacade::Instance().getMutex());
  if (SnakeFacade::Instance().isValid()) {
    SnakeFacade::Instance().getCurrentGame().processUserInput(action, hold);
    if (action == Terminate) {
      SnakeFacade::Instance().invalidateGame();
      SnakeFacade::Instance().terminateGame();
    }
  }
}

GameInfo_t updateCurrentState() {
  GameInfo_t gameInfoCopy;
  if (SnakeFacade::Instance().isValid()) {
    Game& game = SnakeFacade::Instance().getCurrentGame();
    gameInfoCopy = game.gameInfo_;
    gameInfoCopy.field = game.renderField();
  }
  return gameInfoCopy;
}

void freeGameState(GameInfo_t gameInfo) {
  for (int i = 0; i < Game::fieldYSize; ++i) {
    delete[] gameInfo.field[i];
  }
  delete[] gameInfo.field;
}

}  // namespace s21
