#include <QApplication>
#include <string>

#include "s21_menu.h"

int main(int argc, char* argv[]) {
  std::string address = "http://localhost:8080";
  init_library(address.data());

  QApplication app(argc, argv);
  QApplication::setStyle(QStyleFactory::create("Windows"));

  QGraphicsView view;
  s21::Menu menu(&view);
  view.setScene(&menu);
  view.setSceneRect(0, 0, MENU_WIDTH, MENU_HEIGHT);
  view.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  view.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  view.setFixedSize(MENU_WIDTH, MENU_HEIGHT);
  view.setStyleSheet("background-color:#262729");
  view.show();

  app.exec();

  clear_library();
  return 0;
}
