#include "s21_menu.h"

namespace s21 {

Menu::Menu(QGraphicsView* view) : view(view) {
  setSceneRect(0, 0, MENU_WIDTH, MENU_HEIGHT);

  animatedBackground_ = new QLabel;
  animatedBackground_->setMovie(new QMovie(":/gifs/tetback.gif"));
  animatedBackground_->movie()->start();
  animatedBackground_->resize(900, 600);
  animatedBackground_->setScaledContents(true);
  animatedBackground_->setAttribute(Qt::WA_TranslucentBackground);

  QGraphicsProxyWidget* backgroundProxy = addWidget(animatedBackground_);
  backgroundProxy->setPos(0, 0);

  animatedTitle_ = new QLabel;
  QMovie* titleMovie = new QMovie(":/gifs/brickgame.gif");
  titleMovie->setSpeed(50);
  animatedTitle_->setMovie(titleMovie);
  animatedTitle_->movie()->start();
  animatedTitle_->setScaledContents(true);
  animatedTitle_->resize(
      animatedTitle_->movie()->currentImage().size().width() / 3.3,
      animatedTitle_->movie()->currentImage().size().height() / 3.3);
  animatedTitle_->setAttribute(Qt::WA_TranslucentBackground);

  QGraphicsProxyWidget* titleProxy = addWidget(animatedTitle_);
  titleProxy->setPos(180, -50);

  GameType_t* accesibleGamesArray;
  if (get_accesible_games(&accesibleGamesArray).response_status == STATUS_OK) {
    for (int i = 0; i < 3; ++i) {
      QPushButton* button = new QPushButton(accesibleGamesArray[i].name);
      buttons.push_back(button);

      connect(button, &QPushButton::clicked, [this, i]() { startGame(i + 1); });

      button->resize(190, 50);
      button->setFont(QFont("Monospace", 16));
      button->setStyleSheet(
          "QPushButton { background-color: #343436; color: white; border: 2px "
          "solid white; }");

      QGraphicsProxyWidget* buttonProxy = addWidget(button);
      buttonProxy->setPos(450 - 190 / 2, 300 + (i * 70));
    }
    free(accesibleGamesArray);
  }
}

Menu::~Menu() {
  delete animatedBackground_;
  delete animatedTitle_;
  for (auto btn : buttons) {
    delete btn;
  }
}

void Menu::startGame(int id) {
  select_game({nullptr, id});

  GameGraphicsScene* gameGraphicsScene = new GameGraphicsScene(id);
  connect(gameGraphicsScene, &GameGraphicsScene::endGame, this, &Menu::endGame);

  view->setScene(gameGraphicsScene);
  view->setFixedSize(VIEW_WIDTH, VIEW_HEIGHT);
  view->setSceneRect(0, 0, VIEW_WIDTH, VIEW_HEIGHT);
  view->setStyleSheet("background-color:#96b2b5;");
}

void Menu::endGame() {
  delete view->scene();

  // Return to the menu
  view->setScene(this);
  view->setFixedSize(MENU_WIDTH, MENU_HEIGHT);
  view->setStyleSheet("background-color:#262729");
  view->setSceneRect(0, 0, MENU_WIDTH, MENU_HEIGHT);
}

}  // namespace s21
