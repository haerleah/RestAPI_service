#ifndef S21_SNAKE_FACADE_H
#define S21_SNAKE_FACADE_H

#include "s21_snake.h"

namespace s21 {

class SnakeFacade {
 public:
  static SnakeFacade& Instance();

  void initializeGame();
  void terminateGame();
  void invalidateGame();
  bool isValid();
  std::mutex& getMutex();
  GameStatus_t getCurrentGameStatus();
  Game& getCurrentGame();

 private:
  SnakeFacade();
  ~SnakeFacade();
  std::mutex facadeMutex_;

  Game* currentGame_{nullptr};
  bool validationFlag_{false};
};

void userInput(UserAction_t action, bool hold);

GameInfo_t updateCurrentState();

void freeGameState(GameInfo_t gameInfo);

}  // namespace s21

#endif
