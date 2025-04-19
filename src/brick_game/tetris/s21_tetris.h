/**
 * @file s21_tetris.h
 * @brief Game logic header file.
 */

#ifndef S21_TETRIS_H
#define S21_TETRIS_H

// Needs for usleep()
#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED
// Needs for pthread_kill()
#define _REENTRANT

#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define BLANK 0
#define ROWS_FIELD 20
#define COLS_FIELD 10

/**
 * @brief Enum that defines states of FSM
 **/
typedef enum {
  START,
  SPAWN,
  MOVING,
  SHIFTING,
  ATTACHING,
  GAMEOVER,
  EXIT,
  PAUSE
} GameStatus_t;

/**
 * @brief Enum that defines user's actions
 **/
typedef enum {
  Start,
  Pause,
  Terminate,
  Left,
  Right,
  Up,
  Down,
  Action
} UserAction_t;

/**
 * @brief Game data struct
 *
 * @param field Game field represented as 10*20 matrix
 * @param next Next figure
 * @param score Game score
 * @param high_score Highest game score
 * @param level Current level of the game
 * @param speed Current speed of the game
 * @param pause Pause flag
 **/
typedef struct {
  int** field;
  int** next;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
} GameInfo_t;

/**
 * @brief Struct encapsulating data for working with threads.
 **/
typedef struct {
  pthread_t thread;
  pthread_mutex_t mutex;
} ThreadStruct;

/* ---- Singleton-like Getters ---- */
/**
 * @brief Singletone-like function.
 * Provides global access to sub-field.
 * Falling block is located on sub-field, till collision.
 * @return Address of static sub-field
 **/
int*** get_sub_field(void);

/**
 * @brief Singletone-like function.
 * Provides global access to coordintae matrix pointer
 * @return Address of coordinate matrix
 **/
int*** get_coords(void);

/**
 * @brief Singletone-like function.
 * Provides global access to main static GameInfo_t struct.
 * @return Pointer to main static GameInfo_t struct.
 **/
GameInfo_t* get_info_instance(void);

/**
 * @brief Singletone-like function.
 * Provides global access to main static GameStatus_t struct.
 * @return Pointer to main static GameStatus_t struct.
 **/
GameStatus_t* get_status_instance(void);

/**
 * @brief Singletone-like function.
 * Provides global access to timer thread struct.
 * @return Address of static timer thread struct
 **/
ThreadStruct* get_thread_struct_instance();

/**
 * @brief Singletone-like function.
 * Provides global access to the second main thread struct
 * @return Pointer to the static game thread struct
 **/
ThreadStruct* get_game_thread_struct_instance();

/**
 * @brief Singletone-like function.
 * Provides access to current figure index of TetFig matrix.
 * @param index Current index of TetFig matrix to save. Also used as a flag –
 * passing 'NULL' just returns figure index, without saving new one.
 * @return Current figure index of TetFig matrix.
 **/
int get_figure_index(int* index);

/**
 * @brief Singletone-like function.
 * Provides access to game timer.
 * @return Game timer address
 **/
float* get_timer(void);

/**
 * @brief Singletone-like function.
 * Provides global access to main thread pointer.
 * @return Address of the main thread variable.
 **/
pthread_t* get_main_thread();

/**
 * @brief Singletone-like function.
 * Provides global access to a signal-handled flag.
 * @return Address of static bool
 **/
bool* get_signal_handled_flag();

/**
 * @brief Singletone-like function.
 * Provides global access to a flag indicating the first planting of a figure.
 * @return Pointer to the static int
 **/
int* get_first_plant_flag();

/**
 * @brief Singletone-like function.
 * Provides global access to user's action
 * @return Pointer to the static user action enum
 **/
UserAction_t* get_action_instance();

/**
 * @brief Singletone-like function.
 * Provides global access to hold (key) flag
 * @return Pointer to the static bool hold
 **/
bool* get_hold_instance();

/* ---- Main Logic ---- */
/**
 * @brief Initializes the game (memory allocations, seeding, thread creation).
 */
void initialize_game(void);

/**
 * @brief Updates game state and returns its copy. FSM-based approach.
 * @return Copy of current game data struct
 */
GameInfo_t updateCurrentState(void);

/**
 * @brief User's input processing. Also performs the role of FSM.
 * @param action Current user's action
 * @param hold Hold key flag
 **/
void userInput(UserAction_t action, bool hold);

/**
 * @brief Timer thread function.
 * @param arg Pointer to GameInfo_t struct
 * @return Stub as NULL
 */
void* timer_handler(void* arg);

/**
 * @brief Main game thread function (FSM loop).
 * @param arg Unused (NULL)
 * @return Stub as NULL
 */
void* game_handler(void* arg);

/* ---- Signals and Memory Management ---- */
/**
 * @brief Signal handler for SIGUSR1
 * @param signo The signal number
 */
void sig_handler(int signo);

/**
 * @brief Frees up the memory allocated by the game.
 * @param tetris_game Game data struct
 **/
void memfree(GameInfo_t* tetris_game);

/* ---- Gameplay & Mechanics ---- */

/**
 * @brief Initialize function for a new block. Randomly selects a new block.
 * @param tetris_game Pointer to GameInfo_t
 * @param figure_index Figure index from TetFig matrix
 **/
void init_block(GameInfo_t* tetris_game, int* figure_index);

/**
 * @brief Combines main field and sub-field into a single resulting field.
 * @param tetris_game Game data struct
 * @param figure_index Current figure index of TetFig matrix
 **/
void unit_fields(GameInfo_t* tetris_game, int figure_index);

/**
 * @brief Moves current figure down by one step.
 * @param tetris_game Game data struct
 **/
void move_down(GameInfo_t* tetris_game);

/**
 * @brief Moves current figure left by one step.
 * @param tetris_game Game data struct
 **/
void move_left(GameInfo_t* tetris_game);

/**
 * @brief Moves current figure right by one step.
 * @param tetris_game Game data struct
 **/
void move_right(GameInfo_t* tetris_game);

/**
 * @brief Moves current figure instantly down until collision.
 * @param tetris_game Game data struct
 **/
void force_down(GameInfo_t* tetris_game);

/**
 * @brief Places current figure on the sub-field (render).
 **/
void render_field(void);

/**
 * @brief Pauses/resumes the game.
 * @param tetris_game Game data struct
 **/
void pause_game(GameInfo_t* tetris_game);

/**
 * @brief Checks horizontal collision for the current figure.
 * @param tetris_game Game data struct
 * @return true if collided, false otherwise
 **/
bool check_horizontal_collide(GameInfo_t* tetris_game);

/**
 * @brief Checks vertical collision for the current figure.
 * @param tetris_game Game data struct
 * @return true if collided, false otherwise
 **/
bool check_vertical_collide(GameInfo_t* tetris_game);

/**
 * @brief Plants the figure (writes coordinates into static storage).
 * @param tetris_game Game data struct
 * @param cur_rand Random index of TetFig matrix
 **/
void plant_figure(GameInfo_t* tetris_game, int cur_rand);

/**
 * @brief Rotates current figure, if possible.
 * @param tetris_game Game data struct
 * @param figure_index Current figure index of TetFig matrix
 **/
void rotate_block(GameInfo_t* tetris_game, int figure_index);

/**
 * @brief Defines the center of rotation for the current figure.
 * @param px Pointer to X coordinate of rotation center
 * @param py Pointer to Y coordinate of rotation center
 * @param figure_index Current figure index of TetFig matrix
 **/
void get_center(int* px, int* py, int figure_index);

/**
 * @brief Compares current figure with the figure in TetFig matrix.
 * @param figure_1 Current figure
 * @param figure_2 TetFig matrix figure
 * @return true if equal, false otherwise
 **/
bool is_equal(int** figure_1, const int (*figure_2)[4]);

/**
 * @brief Clears the field and sets default score/level/speed.
 * @param tetris_game Game data struct
 **/
void free_field(GameInfo_t* tetris_game);

/**
 * @brief Checks full rows, clears them and counts the score.
 * @param tetris_game Game data struct
 **/
void line_handler(GameInfo_t* tetris_game);

/**
 * @brief Processes scoring, changes speed and level based on score.
 * @param tetris_game Game data struct
 **/
void score_handler(GameInfo_t* tetris_game);

/**
 * @brief Clears row and moves above rows down.
 * @param tetris_game Game data struct
 * @param line_number Number of row to clear
 **/
void destroy_line(GameInfo_t* tetris_game, int line_number);

/**
 * @brief Checks whether the top of the field was reached.
 * @param tetris_game Game data struct
 * @return true if gameover, false otherwise
 **/
bool check_gameover(GameInfo_t* tetris_game);

#endif  // S21_TETRIS_H
