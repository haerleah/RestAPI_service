/**
 * @file s21_desktop.h
 * @brief Desktop graphic user interface header file.
 */

#ifndef S21_DESKTOP_H
#define S21_DESKTOP_H

#include <QCoreApplication>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QPalette>
#include <QTimer>
#include <map>
#include <vector>

extern "C" {
#include "../../client_library/s21_client_library.h"
}

namespace s21 {

#define VIEW_WIDTH 550
#define VIEW_HEIGHT 550

#define FIELD_BLOCKS_WIDTH_COUNT 10
#define FIELD_BLOCKS_HEIGHT_COUNT 20

#define GAME_FIELD_WIDTH (FIELD_BLOCKS_WIDTH_COUNT * BLOCK_WIDTH)
#define GAME_FIELD_HEIGHT (FIELD_BLOCKS_HEIGHT_COUNT * BLOCK_HEIGHT)

#define BLOCK_WIDTH 25
#define BLOCK_HEIGHT 25

#define BLANK 0
#define SNAKE_UP 9
#define SNAKE_DOWN 10
#define SNAKE_LEFT 11
#define SNAKE_RIGHT 12

#define SNAKE_RIGHT_UP 13
#define SNAKE_LEFT_UP 14
#define SNAKE_RIGHT_DOWN 15
#define SNAKE_LEFT_DOWN 16

#define SNAKE_HEAD_UP 18
#define SNAKE_HEAD_DOWN 19
#define SNAKE_HEAD_RIGHT 20
#define SNAKE_HEAD_LEFT 21

#define SNAKE_TAIL_UP 22
#define SNAKE_TAIL_DOWN 23
#define SNAKE_TAIL_RIGHT 24
#define SNAKE_TAIL_LEFT 25
#define FOOD 26

/**
 * @brief Class representing current graphics scene (for a game).
 */
class GameGraphicsScene : public QGraphicsScene {
  Q_OBJECT

 public:
  explicit GameGraphicsScene(int gameId);
  ~GameGraphicsScene();

  /* ---- Rendering Methods ---- */
  void drawStartScreen(GameInfo_t gameInfo);
  void drawGameoverScreen(GameInfo_t gameInfo);
  void drawInterface(GameInfo_t gameInfo);
  void drawField(GameInfo_t gameInfo);
  void drawNextFigure(GameInfo_t gameInfo);
  void drawPause();

  /**
   * @brief Initializes a map of Pixmaps (textures).
   */
  void initializeTextures();

  /* ---- Clear Methods ---- */
  void clearDynamicText();
  void clearField();
  void clearGameOverText();
  void clearStartText();
  void clearPauseText();

 protected:
  /**
   * @brief Override for keyPressEvent, handles user input.
   * @param event The key event.
   */
  void keyPressEvent(QKeyEvent* event) override;

 private:
  int gameId;
  QFont* gameFont;               ///< Font used in the GUI.
  QTimer* drawTimer;             ///< Timer controlling redraw (~60 FPS).
  QGraphicsTextItem* startText;  ///< Text displayed before game starts.
  QGraphicsTextItem* pauseText;  ///< Text displayed during pause.
  std::vector<QGraphicsPixmapItem*> walls;  ///< Window border textures.
  std::vector<QGraphicsPixmapItem*> field;  ///< Field objects textures.
  std::vector<QGraphicsTextItem*>
      staticText;  ///< Static labels (instructions).
  std::vector<QGraphicsTextItem*> dynamicText;  ///< Dynamic text (score, etc.).
  std::vector<QGraphicsTextItem*>
      gameOverText;                 ///< Text for "Game Over" screen.
  std::map<int, QPixmap> textures;  ///< Map of textures per object ID.

 signals:
  void endGame();  ///< Signal emitted when it's time to exit the game.

 private slots:
  void drawGame();  ///< Slot that performs the actual game rendering on each
                    ///< timer tick.
};

}  // namespace s21

#endif  // S21_DESKTOP_H
