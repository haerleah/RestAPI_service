/**
 * @file s21_cli.h
 * @brief CLI graphic user interface header file.
 */
#ifndef S21_CLI_H
#define S21_CLI_H

#define _XOPEN_SOURCE_EXTENDED

#include <locale.h>
#include <menu.h>
#include <ncurses.h>
#include <unistd.h>
#include <wchar.h>

#include "../../client_library/s21_client_library.h"

#define INFO_ROWS 20
#define INFO_COLS 10

#define CLI_ROWS_FIELD 20
#define CLI_COLS_FIELD 10

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

int draw_menu();

/**
 * @brief Initializes the ncurses window.
 **/
void init_screen();

/**
 * @brief Clears up ncurses window.
 **/
void clear_screen();

/**
 * @brief Main game-loop function.
 **/
void cli_game_loop();

/**
 * @brief Defines user's action, depending on the key pressed.
 * @param action Pointer to the enum where the action will be recorded.
 * @param key Code of pressed key.
 * @param hold Pointer to hold
 * @param prev_key Ponter to previous key code container
 * @return Defined action
 **/
UserAction_t get_action(int key, bool* hold, int* prev_key);

/**
 * @brief Checks whether the user is holding down a key.
 * @param prev_key Previous presses key.
 * @param key Current pressed key.
 * @return true if key held down, false otherwise.
 **/
bool check_hold(int prev_key, int key);

/**
 * @brief Draws start screen of the game.
 * @param tetris_game Game data struct.
 * @param game_type Current game type.
 **/
void draw_start_screen();

/**
 * @brief Draws user interface of the game.
 * @param tetris_game Game data struct.
 * @param game_type Current game type.
 **/
void draw_user_interface(GameInfo_t game_info, GameStatus_t game_status,
                         int game_type);

/**
 * @brief Draws field of the game.
 * @param tetris_game Game data struct.
 **/
void draw_field(GameInfo_t tetris_game);

/**
 * @brief Draws gameover screen of the game.
 * @param tetris_game Game data struct.
 * @param game_type Current game type.
 **/
void draw_gameover_screen(GameInfo_t game_info);

/**
 * @brief Prints given string in a middle of a window
 * @param win Window where text will be placed.
 * @param starty Top coordinate of text within window.
 * @param startx Right coordinate of text within window.
 * @param width Width of text.
 * @param string String to be printed.
 * @param color Color of printing text.  
 **/
void print_in_middle(WINDOW* win, int starty, int startx, int width,
                     char* string, chtype color);
/**
 * @brief Frees up GameInfo_t struct.
 * @param current_state Pointer to strcut that will be freed up.
**/
void free_game_state(GameInfo_t* current_state);

#endif
