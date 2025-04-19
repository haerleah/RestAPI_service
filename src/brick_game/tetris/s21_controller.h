/**
 * @file s21_controller.h
 * @brief Controller header file.
 */
#ifndef SRC_TETRIS_CONTROLLER_H
#define SRC_TETRIS_CONTROLLER_H

#include "s21_tetris.h"

/**
 * @brief Gets current model(game) status.
 * @return Current game status.
 **/
GameStatus_t getGameStatus();

/**
 * @brief Updates current state of model.
 * @return Copy of current model's game info struct.
 **/
GameInfo_t updateScene();

/**
 * @brief Sends user action to model.
 * @param action User action.
 * @param hold Hold flag.
 **/
void processUserAction(UserAction_t action, bool hold);

/**
 * @brief Initializes model.
 **/
void initializeGame();

/**
 * @brief Clears up GameInfo_t struct
 * that was allocated before.
 * @param game_info GameInfo_t struct.
 **/
void freeGameInfo(GameInfo_t game_info);

#endif
