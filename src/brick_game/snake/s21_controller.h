/**
 * @file s21_controller.h
 * @brief Controller header file.
 */
#ifndef SRC_SNAKE_CONTROLLER_H
#define SRC_SNAKE_CONTROLLER_H

#include "s21_snake_facade.h"
using namespace s21;

extern "C" {

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
 * @brief Frees up allocated memory.
 **/
void freeGameInfo(GameInfo_t gameInfo);
}

#endif
