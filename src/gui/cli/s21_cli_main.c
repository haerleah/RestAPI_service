#include "s21_cli.h"

int main() {
  init_screen();
  cli_game_loop();
  clear_screen();
  return 0;
}