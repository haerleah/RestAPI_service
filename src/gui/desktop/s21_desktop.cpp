/**
 * @file s21_desktop.cpp
 * @brief Desktop graphic user interface implementation.
 */

#include "s21_desktop.h"

namespace s21 {

GameGraphicsScene::GameGraphicsScene(int gameId) : gameId(gameId) {
  setSceneRect(0, 0, VIEW_WIDTH, VIEW_HEIGHT);

  gameFont = new QFont("Fantasy", 16);

  startText = nullptr;
  pauseText = nullptr;

  for (int i = 0; i < VIEW_WIDTH / BLOCK_WIDTH; ++i) {
    walls.push_back(
        new QGraphicsPixmapItem(QPixmap(":/textures/smallwall.png")));
    walls.back()->setPos(i * BLOCK_WIDTH, 0);
    addItem(walls.back());

    walls.push_back(
        new QGraphicsPixmapItem(QPixmap(":/textures/smallwall.png")));
    walls.back()->setPos(i * BLOCK_WIDTH, VIEW_HEIGHT - BLOCK_HEIGHT);
    addItem(walls.back());
  }

  for (int i = 0; i < VIEW_HEIGHT / BLOCK_HEIGHT; ++i) {
    walls.push_back(
        new QGraphicsPixmapItem(QPixmap(":/textures/smallwall.png")));
    walls.back()->setPos(0, i * BLOCK_HEIGHT);
    addItem(walls.back());

    walls.push_back(
        new QGraphicsPixmapItem(QPixmap(":/textures/smallwall.png")));
    walls.back()->setPos(GAME_FIELD_WIDTH + BLOCK_WIDTH, i * BLOCK_HEIGHT);
    addItem(walls.back());

    walls.push_back(
        new QGraphicsPixmapItem(QPixmap(":/textures/smallwall.png")));
    walls.back()->setPos(VIEW_WIDTH - BLOCK_WIDTH, i * BLOCK_HEIGHT);
    addItem(walls.back());
  }

  staticText.push_back(new QGraphicsTextItem("P - Pause Game"));
  if (gameId == 1) {
    staticText.push_back(new QGraphicsTextItem("R - Rotate Block"));
  }
  staticText.push_back(new QGraphicsTextItem("￩ - Move Left"));
  staticText.push_back(new QGraphicsTextItem("￫ - Move Right"));
  if (gameId != 1) {
    staticText.push_back(new QGraphicsTextItem("￪ - Move Up"));
  }
  staticText.push_back(new QGraphicsTextItem("￬ - Move Down"));
  staticText.push_back(new QGraphicsTextItem("Q - Exit Game"));

  for (size_t i = 0; i < staticText.size(); ++i) {
    staticText[i]->setFont(*gameFont);
    staticText[i]->setPos(GAME_FIELD_WIDTH * 1.3, VIEW_HEIGHT * 0.6 + 30 * i);
    addItem(staticText[i]);
  }

  initializeTextures();

  // Set up timer for ~60FPS redraw
  drawTimer = new QTimer();
  connect(drawTimer, SIGNAL(timeout()), this, SLOT(drawGame()));
  drawTimer->start(17);
}

GameGraphicsScene::~GameGraphicsScene() {
  delete gameFont;
  delete drawTimer;

  clearDynamicText();
  clearField();
  clearGameOverText();
  clearPauseText();
  clearStartText();

  for (auto elem : walls) {
    delete elem;
  }
  for (auto elem : staticText) {
    delete elem;
  }
}

void GameGraphicsScene::drawStartScreen(GameInfo_t gameInfo) {
  drawInterface(gameInfo);
  if (!startText) {
    startText = new QGraphicsTextItem("Press ENTER to start");
    startText->setPos(BLOCK_WIDTH * 1.8, VIEW_HEIGHT / 2);
    startText->setFont(*gameFont);
    addItem(startText);
  }
}

void GameGraphicsScene::drawGameoverScreen(GameInfo_t gameInfo) {
  if (gameOverText.empty()) {
    gameOverText.push_back(new QGraphicsTextItem("GAMEOVER"));
    gameOverText.back()->setPos(BLOCK_WIDTH * 3.5, VIEW_HEIGHT / 2 - 60);

    gameOverText.push_back(new QGraphicsTextItem(
        "Your score is " + QString::number(gameInfo.score)));
    gameOverText.back()->setPos(BLOCK_WIDTH * 3, VIEW_HEIGHT / 2 - 30);

    gameOverText.push_back(new QGraphicsTextItem("again"));
    gameOverText.back()->setPos(BLOCK_WIDTH * 4.6, VIEW_HEIGHT / 2 + 30);

    for (auto textItem : gameOverText) {
      textItem->setFont(*gameFont);
      addItem(textItem);
    }
  }
}

void GameGraphicsScene::drawInterface(GameInfo_t gameInfo) {
  clearDynamicText();
  dynamicText.push_back(new QGraphicsTextItem(
      "High score: " + QString::number(gameInfo.high_score)));
  dynamicText.push_back(
      new QGraphicsTextItem("Score: " + QString::number(gameInfo.score)));
  dynamicText.push_back(
      new QGraphicsTextItem("Level: " + QString::number(gameInfo.level)));
  dynamicText.push_back(
      new QGraphicsTextItem("Speed: " + QString::number(gameInfo.speed)));

  if (gameId == 1) {
    dynamicText.push_back(new QGraphicsTextItem("Next: "));
  }

  for (size_t i = 0; i < dynamicText.size(); ++i) {
    dynamicText[i]->setFont(*gameFont);
    dynamicText[i]->setPos(GAME_FIELD_WIDTH * 1.3, BLOCK_HEIGHT + 30 * i);
    addItem(dynamicText[i]);
  }
}

void GameGraphicsScene::drawField(GameInfo_t gameInfo) {
  clearField();
  if (gameInfo.field) {
    for (int i = 0; i < FIELD_BLOCKS_HEIGHT_COUNT; ++i) {
      for (int j = 0; j < FIELD_BLOCKS_WIDTH_COUNT; ++j) {
        if (gameInfo.field[i][j] != BLANK) {
          field.push_back(
              new QGraphicsPixmapItem(textures[gameInfo.field[i][j]]));
          field.back()->setPos((j + 1) * BLOCK_WIDTH, (i + 1) * BLOCK_HEIGHT);
          addItem(field.back());
        }
      }
    }
    if (gameId == 3) {
      for (int i = 0; i < FIELD_BLOCKS_HEIGHT_COUNT; ++i) {
        field.push_back(
            new QGraphicsPixmapItem(QPixmap(":/textures/smallwall.png")));
        field.back()->setPos(FIELD_BLOCKS_WIDTH_COUNT * BLOCK_WIDTH,
                             (i + 1) * BLOCK_HEIGHT);
        addItem(field.back());
      }
    }
  }
  if (gameId == 1) {
    drawNextFigure(gameInfo);
  }
}

void GameGraphicsScene::drawNextFigure(GameInfo_t gameInfo) {
  if (gameInfo.next) {
    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; ++j) {
        if (gameInfo.next[i][j] != 0) {
          field.push_back(
              new QGraphicsPixmapItem(textures[gameInfo.next[i][j]]));
          field.back()->setPos(
              GAME_FIELD_WIDTH + BLOCK_WIDTH * 5 + (BLOCK_WIDTH * j),
              (GAME_FIELD_HEIGHT * 0.4) + (i * BLOCK_HEIGHT));
          addItem(field.back());
        }
      }
    }
  }
}

void GameGraphicsScene::drawPause() {
  if (!pauseText) {
    pauseText = new QGraphicsTextItem("PAUSE");
    pauseText->setPos(BLOCK_WIDTH * 4.5, VIEW_HEIGHT / 2);
    pauseText->setFont(*gameFont);
    addItem(pauseText);
  }
}

void GameGraphicsScene::initializeTextures() {
  textures.insert({1, QPixmap(":/textures/blueblock.png")});
  textures.insert({2, QPixmap(":/textures/redblock.png")});
  textures.insert({3, QPixmap(":/textures/greenblock.png")});
  textures.insert({4, QPixmap(":/textures/yellowblock.png")});
  textures.insert({5, QPixmap(":/textures/orangeblock.png")});
  textures.insert({6, QPixmap(":/textures/cyanblock.png")});
  textures.insert({7, QPixmap(":/textures/purpleblock.png")});
  textures.insert({8, QPixmap(":/textures/food.png")});
  textures.insert({FOOD, QPixmap(":/textures/food.png")});
  textures.insert({SNAKE_UP, QPixmap(":/textures/up.png")});
  textures.insert({SNAKE_DOWN, QPixmap(":/textures/down.png")});
  textures.insert({SNAKE_LEFT, QPixmap(":/textures/left.png")});
  textures.insert({SNAKE_RIGHT, QPixmap(":/textures/right.png")});
  textures.insert({SNAKE_LEFT_UP, QPixmap(":/textures/leftup.png")});
  textures.insert({SNAKE_LEFT_DOWN, QPixmap(":/textures/leftdown.png")});
  textures.insert({SNAKE_RIGHT_UP, QPixmap(":/textures/rightup.png")});
  textures.insert({SNAKE_RIGHT_DOWN, QPixmap(":/textures/rightdown.png")});
  textures.insert({SNAKE_HEAD_UP, QPixmap(":/textures/headup.png")});
  textures.insert({SNAKE_HEAD_DOWN, QPixmap(":/textures/headdown.png")});
  textures.insert({SNAKE_HEAD_RIGHT, QPixmap(":/textures/headright.png")});
  textures.insert({SNAKE_HEAD_LEFT, QPixmap(":/textures/headleft.png")});
  textures.insert({SNAKE_TAIL_UP, QPixmap(":/textures/tailup.png")});
  textures.insert({SNAKE_TAIL_DOWN, QPixmap(":/textures/taildown.png")});
  textures.insert({SNAKE_TAIL_RIGHT, QPixmap(":/textures/tailright.png")});
  textures.insert({SNAKE_TAIL_LEFT, QPixmap(":/textures/tailleft.png")});
}

void GameGraphicsScene::clearDynamicText() {
  if (!dynamicText.empty()) {
    for (auto item : dynamicText) {
      delete item;
    }
    dynamicText.clear();
  }
}

void GameGraphicsScene::clearField() {
  if (!field.empty()) {
    for (auto item : field) {
      delete item;
    }
    field.clear();
  }
}

void GameGraphicsScene::clearGameOverText() {
  if (!gameOverText.empty()) {
    for (auto item : gameOverText) {
      delete item;
    }
    gameOverText.clear();
  }
}

void GameGraphicsScene::clearStartText() {
  if (startText) {
    delete startText;
    startText = nullptr;
  }
}

void GameGraphicsScene::clearPauseText() {
  if (pauseText) {
    removeItem(pauseText);
    delete pauseText;
    pauseText = nullptr;
  }
}

void GameGraphicsScene::keyPressEvent(QKeyEvent* event) {
  UserAction_t action;
  bool hold = false;

  switch (event->key()) {
    case Qt::Key_Left:
      action = Left;
      hold = event->isAutoRepeat();
      break;
    case Qt::Key_Right:
      action = Right;
      hold = event->isAutoRepeat();
      break;
    case Qt::Key_Up:
      action = Up;
      hold = event->isAutoRepeat();
      break;
    case Qt::Key_Down:
      action = Down;
      hold = event->isAutoRepeat();
      break;
    case Qt::Key_R:
      action = Action;
      break;
    case Qt::Key_Return:
      action = Start;
      break;
    case Qt::Key_P:
      action = Pause;
      break;
    case Qt::Key_Q:
      action = Terminate;
      break;
    default:
      return;  // no recognized key, ignore
  }
  submit_action(action, hold);
}

void GameGraphicsScene::drawGame() {
  clearPauseText();
  GameInfo_t gameInfo;
  GameStatus_t gameStatus;

  get_current_game_status(&gameStatus);

  if (gameStatus != PAUSE) {
    if (get_current_state(&gameInfo).response_status != STATUS_OK) {
      gameStatus = EXIT;
    }

    if (gameStatus == EXIT) {
      emit endGame();
    }

    if (gameStatus == START) {
      drawStartScreen(gameInfo);
    } else if (gameStatus == GAMEOVER) {
      drawGameoverScreen(gameInfo);
    } else if (gameStatus != SPAWN && gameStatus != EXIT) {
      clearGameOverText();
      clearStartText();
      drawInterface(gameInfo);
      drawField(gameInfo);
    }

    if (gameInfo.field) {
      for (int i = 0; i < FIELD_BLOCKS_HEIGHT_COUNT; ++i) {
        free(gameInfo.field[i]);
      }
      free(gameInfo.field);
      gameInfo.field = nullptr;
    }
    if (gameInfo.next) {
      for (int i = 0; i < 4; ++i) {
        free(gameInfo.next[i]);
      }
      free(gameInfo.next);
      gameInfo.next = nullptr;
    }

  } else {
    drawPause();
  }
}

}  // namespace s21
