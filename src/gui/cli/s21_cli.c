/**
 * @file s21_cli.c
 * @brief CLI graphic user interface implementation.
 */
#include "s21_cli.h"

int draw_menu() {
  ITEM **my_items;
  int c;
  MENU *my_menu;
  WINDOW *my_menu_win;
  int n_choices, i;
  GameType_t *accesible_games;
  Response_t response = get_accesible_games(&accesible_games);
  if (response.response_status != 200) {
    return -1;
  }

  n_choices = 3;
  my_items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
  for (i = 0; i < n_choices; ++i)
    my_items[i] = new_item(accesible_games[i].name, "");
  my_items[n_choices] = new_item("Exit", "");

  /* Crate menu */
  my_menu = new_menu((ITEM **)my_items);

  /* Create the window to be associated with the menu */
  my_menu_win = newwin(10, 40, 4, 4);
  keypad(my_menu_win, TRUE);

  /* Set main window and sub window */
  set_menu_win(my_menu, my_menu_win);
  set_menu_sub(my_menu, derwin(my_menu_win, 6, 38, 3, 1));

  /* Set menu mark to the string " * " */
  set_menu_mark(my_menu, " - ");

  /* Print a border around the main window and print a title */
  box(my_menu_win, 0, 0);
  print_in_middle(my_menu_win, 1, 0, 40, "Select game", COLOR_PAIR(6));
  mvwaddch(my_menu_win, 2, 0, ACS_LTEE);
  mvwhline(my_menu_win, 2, 1, ACS_HLINE, 38);
  mvwaddch(my_menu_win, 2, 39, ACS_RTEE);
  refresh();

  /* Post the menu */
  post_menu(my_menu);
  wrefresh(my_menu_win);
  int selected_game = 0;
  while (selected_game == 0) {
    c = wgetch(my_menu_win);
    switch (c) {
      case KEY_DOWN:
        menu_driver(my_menu, REQ_DOWN_ITEM);
        break;
      case KEY_UP:
        menu_driver(my_menu, REQ_UP_ITEM);
        break;
      case 10: /* Enter */
        for (int i = 0; i < n_choices; ++i) {
          if (strcmp(accesible_games[i].name,
                     item_name(current_item(my_menu))) == 0) {
            selected_game = i + 1;
            break;
          }
          if (strcmp(item_name(current_item(my_menu)), "Exit") == 0) {
            selected_game = -1;
            break;
          }
        }
        break;
    }
    wrefresh(my_menu_win);
  }

  /* Unpost and free all the memory taken up */
  unpost_menu(my_menu);
  free_menu(my_menu);
  for (i = 0; i < n_choices; ++i) {
    free_item(my_items[i]);
    free(accesible_games[i].name);
  }
  return selected_game;
}

void init_screen() {
  initscr();
  nodelay(stdscr, true);
  start_color();
  init_pair(1, COLOR_BLUE, COLOR_BLACK);
  init_pair(2, COLOR_CYAN, COLOR_BLACK);
  init_pair(3, COLOR_GREEN, COLOR_BLACK);
  init_pair(4, COLOR_RED, COLOR_BLACK);
  init_pair(5, COLOR_YELLOW, COLOR_BLACK);
  init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(7, COLOR_WHITE, COLOR_BLACK);
  init_pair(8, COLOR_BLACK, COLOR_BLACK);
  init_pair(26, COLOR_RED, COLOR_BLACK);

  cbreak();
  noecho();
  curs_set(0);
  keypad(stdscr, true);
  timeout(50);
}

void clear_screen() {
  printw("\n\nClosing BrickGameV2.0...\n\n");
  refresh();
  clear();
  refresh();
  endwin();
  exit_curses(0);
}

void cli_game_loop() {
  init_library("http://localhost:8080");
  GameInfo_t game_info;
  int key = -1;
  int prev_key = -1;
  bool hold = false;
  int selected_game = draw_menu();
  while (selected_game != -1) {
    GameType_t selected_type = {"", selected_game};
    select_game(selected_type);
    bool break_flag = true;
    while (break_flag) {
      GameStatus_t game_status;
      if (get_current_game_status(&game_status).response_status != 200) {
        game_status = EXIT;
      }
      key = getch();
      if (key != ERR) {
        UserAction_t action = get_action(key, &hold, &prev_key);
        if (submit_action(action, hold).response_status != 200) {
          game_status = EXIT;
        }
      } else {
        prev_key = -1;
      }
      if (game_status != PAUSE) {
        get_current_state(&game_info);
        if (game_status == START) {
          draw_start_screen();
          draw_user_interface(game_info, game_status, selected_game);
        } else if (game_status == GAMEOVER) {
          draw_gameover_screen(game_info);
          draw_user_interface(game_info, game_status, selected_game);
        } else if (game_status != SPAWN) {
          draw_user_interface(game_info, game_status, selected_game);
          draw_field(game_info);
        }
        refresh();
        if (game_status == EXIT) {
          break_flag = false;
        }
        free_game_state(&game_info);
      } else {
        mvprintw(1 + CLI_ROWS_FIELD / 2, CLI_COLS_FIELD - 1, "PAUSE");
        move(CLI_ROWS_FIELD + 1, CLI_COLS_FIELD * 2 + CLI_COLS_FIELD * 2 + 3);
      }
    }
    clear();
    selected_game = draw_menu();
  }
  clear_library();
}

UserAction_t get_action(int key, bool *hold, int *prev_key) {
  UserAction_t result;
  if (key == '\n') {
    result = Start;
  } else if (key == 'r' || key == 'R') {
    result = Action;
  } else if (key == 'q' || key == 'Q') {
    result = Terminate;
  } else if (key == KEY_LEFT) {
    result = Left;
    *hold = check_hold(*prev_key, key);
  } else if (key == KEY_RIGHT) {
    result = Right;
    *hold = check_hold(*prev_key, key);
  } else if (key == KEY_DOWN) {
    result = Down;
    *hold = check_hold(*prev_key, key);
  } else if (key == KEY_UP) {
    result = Up;
    *hold = check_hold(*prev_key, key);
  } else if (key == 'p' || key == 'P') {
    result = Pause;
  } else {
    *hold = false;
  }
  *prev_key = key;
  return result;
}

void draw_user_interface(GameInfo_t game_info, GameStatus_t game_status,
                         int game_type) {
  mvhline(0, 0, ACS_HLINE, CLI_COLS_FIELD * 2 + INFO_COLS * 2 + 2);
  mvhline(CLI_ROWS_FIELD + 1, 0, ACS_HLINE,
          CLI_COLS_FIELD * 2 + INFO_COLS * 2 + 2);
  mvvline(1, 0, ACS_VLINE, CLI_ROWS_FIELD);
  mvvline(1, CLI_COLS_FIELD * 2 + 1, ACS_VLINE, CLI_ROWS_FIELD);
  mvvline(1, CLI_COLS_FIELD * 2 + INFO_COLS * 2 + 2, ACS_VLINE, CLI_ROWS_FIELD);

  mvaddch(0, 0, ACS_ULCORNER);
  mvaddch(0, CLI_COLS_FIELD * 2 + INFO_COLS * 2 + 2, ACS_URCORNER);
  mvaddch(CLI_ROWS_FIELD + 1, 0, ACS_LLCORNER);
  mvaddch(CLI_ROWS_FIELD + 1, CLI_COLS_FIELD * 2 + INFO_COLS * 2 + 2,
          ACS_LRCORNER);

  mvaddch(0, CLI_COLS_FIELD * 2 + 1, ACS_TTEE);
  mvaddch(CLI_ROWS_FIELD + 1, CLI_COLS_FIELD * 2 + 1, ACS_BTEE);

  move(CLI_ROWS_FIELD + 1, CLI_COLS_FIELD * 2 + INFO_COLS * 2 + 3);

  mvprintw(2, CLI_COLS_FIELD * 2 + 3, "HIGH SCORE: %d", game_info.high_score);
  mvprintw(4, CLI_COLS_FIELD * 2 + 3, "SCORE: %4d", game_info.score);
  mvprintw(6, CLI_COLS_FIELD * 2 + 3, "LEVEL: %d", game_info.level);
  mvprintw(8, CLI_COLS_FIELD * 2 + 3, "SPEED: %d", game_info.speed);
  if (game_type == 1) {
    mvprintw(10, CLI_COLS_FIELD * 2 + 3, "NEXT");
    if (game_status != START && game_info.next != NULL) {
      for (int row = 0; row < 2; row++)
        for (int col = 0; col < 4; col++) {
          if (game_info.next[row][col]) {
            attron(COLOR_PAIR(game_info.next[row][col]));
            mvaddch(11 + row, CLI_COLS_FIELD * 2 + 6 * 2 + col * 2,
                    ACS_CKBOARD);
            mvaddch(11 + row, CLI_COLS_FIELD * 2 + 6 * 2 + col * 2 + 1,
                    ACS_CKBOARD);
            attroff(COLOR_PAIR(game_info.next[row][col]));
          } else {
            attron(COLOR_PAIR(8));
            mvaddch(11 + row, CLI_COLS_FIELD * 2 + 6 * 2 + col * 2,
                    ACS_CKBOARD);
            mvaddch(11 + row, CLI_COLS_FIELD * 2 + 6 * 2 + col * 2 + 1,
                    ACS_CKBOARD);
            attroff(COLOR_PAIR(8));
          }
        }
    }
  }
  mvprintw(15, CLI_COLS_FIELD * 2 + 5, "P - Pause game");
  mvaddch(16, CLI_COLS_FIELD * 2 + 5, ACS_LARROW);
  addstr(" - Move left");
  mvaddch(17, CLI_COLS_FIELD * 2 + 5, ACS_RARROW);
  addstr(" - Move right");
  mvaddch(18, CLI_COLS_FIELD * 2 + 5, ACS_DARROW);
  addstr(" - Move down");
  if (game_type == 1) {
    mvaddstr(19, CLI_COLS_FIELD * 2 + 3, "  R - Rotate");
  } else {
    mvaddch(19, CLI_COLS_FIELD * 2 + 5, ACS_UARROW);
    addstr(" - Move Up");
  }
  mvaddstr(20, CLI_COLS_FIELD * 2 + 5, "Q  - Exit game");
  move(CLI_ROWS_FIELD + 1, CLI_COLS_FIELD * 2 + INFO_COLS * 2 + 3);
}

void draw_field(GameInfo_t game_info) {
  chtype left_bar = '[' | A_DIM;
  chtype right_bar = ']' | A_DIM;
  int color_pair;
  if (game_info.field != NULL) {
    for (int row = 0; row < CLI_ROWS_FIELD; row++) {
      for (int col = 0; col < CLI_COLS_FIELD; col++)
        if (game_info.field[row][col] != 0) {
          color_pair = game_info.field[row][col];
          if (color_pair > 8 && color_pair < 26) {
            color_pair = 1;
          }
          attron(COLOR_PAIR(color_pair));
          mvaddch(1 + row, 1 + col * 2, ACS_CKBOARD);
          mvaddch(1 + row, 1 + col * 2 + 1, ACS_CKBOARD);
          attroff(COLOR_PAIR(color_pair));
        } else {
          attron(COLOR_PAIR(8));
          mvaddch(1 + row, 1 + col * 2, left_bar);
          mvaddch(1 + row, 1 + col * 2 + 1, right_bar);

          attroff(COLOR_PAIR(8));
        }
    }
  }
  move(CLI_ROWS_FIELD + 1, CLI_COLS_FIELD * 2 + CLI_COLS_FIELD * 2 + 3);
}

bool check_hold(int prev_key, int key) {
  bool result = false;
  if (prev_key == key && key != -1) {
    result = true;
  }
  return result;
}

void draw_start_screen() {
  mvprintw(1 + CLI_ROWS_FIELD / 2, 1, "Press ENTER to start");
  move(CLI_ROWS_FIELD + 1, CLI_COLS_FIELD * 2 + INFO_COLS * 2 + 3);
}

void draw_gameover_screen(GameInfo_t game_info) {
  draw_field(game_info);

  mvprintw(CLI_ROWS_FIELD / 2, 7, "GAMEOVER");
  mvprintw(CLI_ROWS_FIELD / 2 + 1, 5, "Press  ENTER");
  mvprintw(CLI_ROWS_FIELD / 2 + 2, 9, "again");
  mvprintw(CLI_ROWS_FIELD / 2 + 4, 3, "Your Score is %d", game_info.score);

  move(CLI_ROWS_FIELD + 1, CLI_COLS_FIELD * 2 + INFO_COLS * 2 + 3);
}

void print_in_middle(WINDOW *win, int starty, int startx, int width,
                     char *string, chtype color) {
  int length, x, y;
  float temp;

  if (win == NULL) win = stdscr;
  getyx(win, y, x);
  if (startx != 0) x = startx;
  if (starty != 0) y = starty;
  if (width == 0) width = 80;

  length = strlen(string);
  temp = (width - length) / 2;
  x = startx + (int)temp;
  wattron(win, color);
  mvwprintw(win, y, x, "%s", string);
  wattroff(win, color);
  refresh();
}

void free_game_state(GameInfo_t *current_state) {
  if (current_state->field != NULL) {
    for (int i = 0; i < CLI_ROWS_FIELD; ++i) {
      free(current_state->field[i]);
    }
    free(current_state->field);
    current_state->field = NULL;
  }

  if (current_state->next != NULL) {
    for (int i = 0; i < 4; ++i) {
      free(current_state->next[i]);
    }
    free(current_state->next);
    current_state->next = NULL;
  }
}
