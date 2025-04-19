/**
 * @file s21_snake.cpp
 * @brief Game logic source code
 */

#include "s21_snake.h"

namespace s21 {

/* -------------------------------------------------------------------------- */
/*                         Game Class Implementation                          */
/* -------------------------------------------------------------------------- */

Game::Game() {
  gameInfo_.field = nullptr;
  gameInfo_.next = nullptr;
  gameInfo_.score = 0;
  gameInfo_.high_score = 0;
  gameInfo_.level = 1;
  gameInfo_.speed = 1;
  gameInfo_.pause = 0;

  currentGameStatus_ = START;
  gameTimer_ = 0.0f;

  snake_ = new Snake(this);
  food_ = new Food();
  food_->setAssociatedGame(this);

  std::ifstream file("snake_score");
  if (file.eof()) {
    gameInfo_.high_score = 0;
  } else {
    file >> gameInfo_.high_score;
  }
  file.close();

  holdFlag_ = false;
  actionUsedFlag_ = false;
  userAction_ = Action;

  timerThread_ = new std::thread(&Game::processTimer, this);
  gameThread_ = new std::thread(&Game::handleGameProcessing, this);
}

Game::~Game() {
  currentGameStatus_ = EXIT;
  if (timerThread_->joinable()) {
    timerThread_->join();
  }
  if (gameThread_->joinable()) {
    gameThread_->join();
  }
  delete timerThread_;
  delete gameThread_;
  delete snake_;
  delete food_;
}

void Game::processUserInput(UserAction_t& action, bool hold) {
  std::lock_guard<std::mutex> guard(gameMutex_);
  userAction_ = action;
  holdFlag_ = hold;
  actionUsedFlag_ = false;
}

void Game::gameStart() {
  srand(time(nullptr));
  gameInfo_.score = 0;
  gameInfo_.level = 1;
  gameInfo_.speed = 1;
  gameInfo_.pause = 0;
  snake_->reset();
  food_->reset();
}

void Game::scoreHandler() {
  gameInfo_.score += 1;
  if (gameInfo_.score > gameInfo_.high_score) {
    gameInfo_.high_score = gameInfo_.score;
    std::ofstream file("snake_score");
    file << gameInfo_.score;
    file.close();
  }
  if (gameInfo_.score % 5 == 0 && gameInfo_.level < 10) {
    gameInfo_.level += 1;
    if (gameInfo_.level % 2 == 0 && gameInfo_.level > 2) {
      gameInfo_.speed += 1;
    }
  }
}

void Game::pauseGame() { gameInfo_.pause = !gameInfo_.pause; }

int** Game::renderField() {
  int** field = new int*[fieldYSize];
  for (int i = 0; i < fieldYSize; ++i) {
    field[i] = new int[fieldXSize]();
  }

  if (currentGameStatus_ != START && currentGameStatus_ != SPAWN) {
    field[food_->rowCoord_][food_->colCoord_] = FOOD;

    auto bodyIterator = ++snake_->snakeBody_.begin();
    for (auto elem : snake_->snakeBody_) {
      int row = elem->getRowCoord();
      int col = elem->getColCoord();
      if (row >= 0 && row < fieldYSize && col >= 0 && col < fieldXSize) {
        switch (elem->getElemDirection()) {
          case Snake::up:
            if (elem->isHead()) {
              field[row][col] = SNAKE_HEAD_UP;
            } else if (elem == snake_->snakeBody_.back()) {
              field[row][col] = SNAKE_TAIL_UP;
            } else {
              field[row][col] = SNAKE_UP;
            }
            break;
          case Snake::down:
            if (elem->isHead()) {
              field[row][col] = SNAKE_HEAD_DOWN;
            } else if (elem == snake_->snakeBody_.back()) {
              field[row][col] = SNAKE_TAIL_DOWN;
            } else {
              field[row][col] = SNAKE_DOWN;
            }
            break;
          case Snake::left:
            if (elem->isHead()) {
              field[row][col] = SNAKE_HEAD_LEFT;
            } else if (elem == snake_->snakeBody_.back()) {
              field[row][col] = SNAKE_TAIL_LEFT;
            } else {
              field[row][col] = SNAKE_LEFT;
            }
            break;
          case Snake::right:
            if (elem->isHead()) {
              field[row][col] = SNAKE_HEAD_RIGHT;
            } else if (elem == snake_->snakeBody_.back()) {
              field[row][col] = SNAKE_TAIL_RIGHT;
            } else {
              field[row][col] = SNAKE_RIGHT;
            }
            break;
        }

        if (bodyIterator != snake_->snakeBody_.end() &&
            (*bodyIterator)->getElemDirection() != elem->getElemDirection()) {
          field[row][col] = snake_->bodyRotationType(elem, (*bodyIterator));
        }
      }
      ++bodyIterator;
    }
  }
  return field;
}

void Game::processTimer() {
  while (currentGameStatus_ != EXIT) {
    {
      std::lock_guard<std::mutex> guard(timerMutex_);
      if (holdFlag_) {
        gameTimer_ += 0.5;
      } else {
        if (gameInfo_.speed == 1) {
          gameTimer_ += 150 * 1e-3;
        } else {
          gameTimer_ += (75 * gameInfo_.speed * 1e-3);
        }
      }
    }
    std::this_thread::sleep_for(std::chrono::microseconds((int)(5 * 10e3)));
  }
}

GameStatus_t Game::getStatus() { return currentGameStatus_; }

void Game::handleGameProcessing() {
  static bool rotateFlag = true;

  while (currentGameStatus_ != EXIT) {
    switch (currentGameStatus_) {
      case START: {
        std::lock_guard<std::mutex> guard(gameMutex_);
        if (userAction_ == Start) {
          currentGameStatus_ = SPAWN;
          actionUsedFlag_ = true;
        } else if (userAction_ == Terminate) {
          currentGameStatus_ = EXIT;
          actionUsedFlag_ = true;
        }
      } break;
      case SPAWN:
        gameStart();
        currentGameStatus_ = MOVING;
        break;
      case MOVING: {
        {
          std::lock_guard<std::mutex> guard(gameMutex_);
          switch (userAction_) {
            case Left:
              snake_->moveLeft(rotateFlag);
              actionUsedFlag_ = true;
              break;
            case Right:
              snake_->moveRight(rotateFlag);
              actionUsedFlag_ = true;
              break;
            case Up:
              snake_->moveUp(rotateFlag);
              actionUsedFlag_ = true;
              break;
            case Down:
              snake_->moveDown(rotateFlag);
              actionUsedFlag_ = true;
              break;
            case Terminate:
              currentGameStatus_ = EXIT;
              actionUsedFlag_ = true;
              break;
            case Pause:
              pauseGame();
              currentGameStatus_ = PAUSE;
              actionUsedFlag_ = true;
              break;
            default:
              break;
          }
        }
        __attribute__((fallthrough));
      }

      case SHIFTING: {
        std::lock_guard<std::mutex> guard(timerMutex_);
        if (gameTimer_ > 1.5 && gameInfo_.pause != 1) {
          rotateFlag = true;
          if (!snake_->moveForward()) {
            currentGameStatus_ = GAMEOVER;
          } else if (snake_->attachFood()) {
            currentGameStatus_ = ATTACHING;
          } else {
            currentGameStatus_ = MOVING;
          }
          gameTimer_ = 0;
        }
      } break;

      case ATTACHING:
        scoreHandler();
        food_->spawnFood();
        if (gameInfo_.score == 200) {
          currentGameStatus_ = GAMEOVER;
        } else {
          currentGameStatus_ = MOVING;
        }
        break;

      case GAMEOVER: {
        if (userAction_ == Start) {
          currentGameStatus_ = SPAWN;
        } else if (userAction_ == Terminate) {
          currentGameStatus_ = EXIT;
        }
      } break;

      case PAUSE: {
        {
          std::lock_guard<std::mutex> guard(gameMutex_);
          if (userAction_ == Pause) {
            pauseGame();
            actionUsedFlag_ = true;
          }
        }
        if (gameInfo_.pause == 0) {
          currentGameStatus_ = MOVING;
        }
      } break;

      case EXIT:
        break;
    }

    {
      std::lock_guard<std::mutex> guard(gameMutex_);
      if (actionUsedFlag_) {
        userAction_ = Action;
      }
      holdFlag_ = false;
    }

    std::this_thread::sleep_for(std::chrono::microseconds((int)(20 * 10e3)));
  }
}

/* -------------------------------------------------------------------------- */
/*                         Snake Class Implementation                         */
/* -------------------------------------------------------------------------- */

Snake::Snake(Game* associatedGame) {
  currentGame = associatedGame;
  snakeDirection_ = right;
  snakeBody_.reserve(startSize);
}

Snake::~Snake() {
  for (auto element : snakeBody_) {
    delete element;
  }
}

void Snake::moveLeft(bool& rotateFlag) {
  if ((snakeDirection_ == up || snakeDirection_ == down) && rotateFlag) {
    snakeDirection_ = left;
    snakeBody_[0]->elemDirection_ = snakeDirection_;
    rotateFlag = false;
  }
}

void Snake::moveRight(bool& rotateFlag) {
  if ((snakeDirection_ == up || snakeDirection_ == down) && rotateFlag) {
    snakeDirection_ = right;
    snakeBody_[0]->elemDirection_ = snakeDirection_;
    rotateFlag = false;
  }
}

void Snake::moveUp(bool& rotateFlag) {
  if ((snakeDirection_ == right || snakeDirection_ == left) && rotateFlag) {
    snakeDirection_ = up;
    snakeBody_[0]->elemDirection_ = snakeDirection_;
    rotateFlag = false;
  }
}

void Snake::moveDown(bool& rotateFlag) {
  if ((snakeDirection_ == right || snakeDirection_ == left) && rotateFlag) {
    snakeDirection_ = down;
    snakeBody_[0]->elemDirection_ = snakeDirection_;
    rotateFlag = false;
  }
}

bool Snake::checkCollision() {
  for (size_t i = 1; i < snakeBody_.size(); ++i) {
    if (snakeBody_[0]->getRowCoord() == snakeBody_[i]->getRowCoord() &&
        snakeBody_[0]->getColCoord() == snakeBody_[i]->getColCoord()) {
      return true;
    }
  }

  if (snakeBody_[0]->getRowCoord() < 0 || snakeBody_[0]->getRowCoord() > 19 ||
      snakeBody_[0]->getColCoord() < 0 || snakeBody_[0]->getColCoord() > 9) {
    return true;
  }
  return false;
}

bool Snake::attachFood() {
  for (auto elem : snakeBody_) {
    if (elem->getRowCoord() == currentGame->food_->getRowCoord() &&
        elem->getColCoord() == currentGame->food_->getColCoord()) {
      return true;
    }
  }
  return false;
}

bool Snake::moveForward() {
  bool result = true;

  snakeBody_.insert(
      snakeBody_.begin() + 1,
      new SnakeElement(snakeBody_[0]->rowCoord_, snakeBody_[0]->colCoord_,
                       snakeDirection_));

  switch (snakeDirection_) {
    case up:
      snakeBody_[0]->rowCoord_ -= 1;
      if (checkCollision()) {
        snakeBody_[0]->rowCoord_ += 1;
        result = false;
      }
      break;
    case down:
      snakeBody_[0]->rowCoord_ += 1;
      if (checkCollision()) {
        snakeBody_[0]->rowCoord_ -= 1;
        result = false;
      }
      break;
    case left:
      snakeBody_[0]->colCoord_ -= 1;
      if (checkCollision()) {
        snakeBody_[0]->colCoord_ += 1;
        result = false;
      }
      break;
    case right:
      snakeBody_[0]->colCoord_ += 1;
      if (checkCollision()) {
        snakeBody_[0]->colCoord_ -= 1;
        result = false;
      }
      break;
  }

  if (!attachFood() && result) {
    delete snakeBody_.back();
    snakeBody_.pop_back();
  }
  return result;
}

void Snake::reset() {
  for (auto elem : snakeBody_) {
    delete elem;
  }
  snakeBody_.clear();

  for (int i = startSize; i > 0; --i) {
    snakeBody_.push_back(new SnakeElement(10, i, right));
  }
  snakeBody_[0]->isHead_ = true;
  snakeDirection_ = right;
}

int Snake::bodyRotationType(SnakeElement* curElem, SnakeElement* prevElem) {
  direction cur = curElem->getElemDirection();
  direction prev = prevElem->getElemDirection();
  int result = 0;

  switch (cur) {
    case up:
      if (curElem->isHead()) {
        if (prev == right) {
          result = SNAKE_HEAD_RIGHT;
        } else if (prev == left) {
          result = SNAKE_HEAD_LEFT;
        }
      } else {
        if (prev == right) {
          result = SNAKE_RIGHT_UP;
        } else if (prev == left) {
          result = SNAKE_LEFT_UP;
        }
      }
      break;

    case down:
      if (curElem->isHead()) {
        if (prev == right) {
          result = SNAKE_HEAD_RIGHT;
        } else if (prev == left) {
          result = SNAKE_HEAD_LEFT;
        }
      } else {
        if (prev == right) {
          result = SNAKE_RIGHT_DOWN;
        } else if (prev == left) {
          result = SNAKE_LEFT_DOWN;
        }
      }
      break;

    case right:
      if (curElem->isHead()) {
        if (prev == up) {
          result = SNAKE_HEAD_UP;
        } else if (prev == down) {
          result = SNAKE_HEAD_DOWN;
        }
      } else {
        if (prev == up) {
          result = SNAKE_LEFT_DOWN;
        } else if (prev == down) {
          result = SNAKE_LEFT_UP;
        }
      }
      break;

    case left:
      if (curElem->isHead()) {
        if (prev == up) {
          result = SNAKE_HEAD_UP;
        } else if (prev == down) {
          result = SNAKE_HEAD_DOWN;
        }
      } else {
        if (prev == up) {
          result = SNAKE_RIGHT_DOWN;
        } else if (prev == down) {
          result = SNAKE_RIGHT_UP;
        }
      }
      break;
  }
  return result;
}

/* -------------------------------------------------------------------------- */
/*                       Food Class Implementation                            */
/* -------------------------------------------------------------------------- */

void Food::spawnFood() {
  do {
    rowCoord_ = rand() % Game::fieldYSize;
    colCoord_ = rand() % Game::fieldXSize;
  } while (currentGame_->snake_->attachFood());
}

void Food::reset() {
  rowCoord_ = Game::fieldYSize / 2;
  colCoord_ = (Game::fieldXSize / 2) + 2;
}

void Food::setAssociatedGame(Game* associatedGame) {
  currentGame_ = associatedGame;
}

int Food::getRowCoord() { return rowCoord_; }

int Food::getColCoord() { return colCoord_; }

/* -------------------------------------------------------------------------- */
/*                     SnakeElement Class Implementation                      */
/* -------------------------------------------------------------------------- */

bool SnakeElement::isHead() { return isHead_; }

int SnakeElement::getRowCoord() { return rowCoord_; }

int SnakeElement::getColCoord() { return colCoord_; }

Snake::direction SnakeElement::getElemDirection() { return elemDirection_; }

}  // namespace s21
