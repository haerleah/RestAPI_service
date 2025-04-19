#ifndef MENU_H
#define MENU_H

#include <QApplication>
#include <QCoreApplication>
#include <QGraphicsProxyWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLabel>
#include <QMovie>
#include <QObject>
#include <QProcess>
#include <QPushButton>
#include <QStyleFactory>
#include <vector>

#include "s21_desktop.h"

extern "C" {
#include "../../client_library/s21_client_library.h"
}

#define MENU_WIDTH 900
#define MENU_HEIGHT 600

namespace s21 {

class Menu : public QGraphicsScene {
 public:
  explicit Menu(QGraphicsView* view);
  ~Menu();

 public slots:
  /**
   * @brief Starts a game with the given id.
   * @param id game type
   */
  void startGame(int id);

  /**
   * @brief Slot called when the game ends.
   * Restores menu scene in the QGraphicsView.
   */
  void endGame();

 private:
  QGraphicsView* view;  ///< View that shows this scene
  QLabel* animatedTitle_{nullptr};
  QLabel* animatedBackground_{nullptr};
  std::vector<QPushButton*> buttons;
};

}  // namespace s21

#endif  // MENU_H
