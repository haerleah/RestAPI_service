/**
 * @file s21_snake.h
 * @brief Game logic header file.
 */
#ifndef SRC_SNAKE_H
#define SRC_SNAKE_H

#include <atomic>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <map>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

namespace s21 {

class SnakeElement;
class Game;

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
 * @brief Defines states of FSM
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
 * @brief Defines user's actions
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
 * @param field Not used
 * @param next Not used
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

/* ==== Free Functions ==== */
/**
 * @brief User's input processing.
 * Also performs the role of FSM.
 * @param action Current user's action
 * @param hold Hold key flag
 **/
void userInput(UserAction_t action, bool hold);

/**
 * @brief Updates current state of the game and renders game field.
 * @return Copy of current game data struct.
 **/
GameInfo_t updateCurrentState();

/**
 * @brief Class representing food object.
 */
class Food {
 public:
  Food() : rowCoord_(0), colCoord_(0) {}
  Food(int x, int y) : rowCoord_(x), colCoord_(y) {}
  ~Food() = default;

  void spawnFood();  ///< Respawns food object in random position.
  void reset();      ///< Resets food object to default position.
  void setAssociatedGame(Game* associatedGame);

  int getRowCoord();  ///< Row coord getter
  int getColCoord();  ///< Col coord getter

 private:
  friend class Game;
  Game* currentGame_{nullptr};
  int rowCoord_{0};
  int colCoord_{0};
};

/**
 * @brief Class representing snake.
 */
class Snake {
 public:
  enum direction { up, down, left, right };

  Snake(Game* associatedGame);
  ~Snake();

  void moveLeft(bool& rotateFlag);
  void moveRight(bool& rotateFlag);
  void moveUp(bool& rotateFlag);
  void moveDown(bool& rotateFlag);

  bool checkCollision();
  bool attachFood();
  bool moveForward();
  void reset();

  /**
   * @brief Determines the type of rotation of the snake body,
   *        between the current and previous body segment.
   * @param curElem Current element of snake body
   * @param prevElem Previous element of snake body
   * @return Body rotation type (macros define these types)
   */
  int bodyRotationType(SnakeElement* curElem, SnakeElement* prevElem);

 private:
  friend class Game;
  Game* currentGame{nullptr};
  direction snakeDirection_{right};
  std::vector<SnakeElement*> snakeBody_;
  static const int startSize = 4;
};

/**
 * @brief Class representing single snake body element.
 */
class SnakeElement {
 public:
  SnakeElement() = default;
  SnakeElement(int x, int y, Snake::direction direction)
      : rowCoord_(x), colCoord_(y), elemDirection_(direction) {}
  ~SnakeElement() = default;

  bool isHead();                        ///< Checks if element is head
  int getRowCoord();                    ///< Row coord getter
  int getColCoord();                    ///< Col coord getter
  Snake::direction getElemDirection();  ///< Element direction getter

 private:
  friend class Snake;
  int rowCoord_{0};
  int colCoord_{0};
  Snake::direction elemDirection_{Snake::right};
  bool isHead_{false};
};

/**
 * @brief Class representing the whole game.
 */
class Game {
 public:
  static const int fieldXSize = 10;  ///< Col size of game field
  static const int fieldYSize = 20;  ///< Row size of game field

  Game();
  ~Game();

  Game(Game& other) = delete;
  Game& operator=(Game& other) = delete;

  friend GameInfo_t updateCurrentState();

  void processUserInput(UserAction_t& action, bool hold);
  void gameStart();
  void scoreHandler();
  void pauseGame();
  int** renderField();
  void processTimer();

  GameStatus_t getStatus();

#ifdef TESTING
  float& getTimer() { return gameTimer_; }
  std::mutex& getMutex() { return timerMutex_; }
#endif

 private:
  friend class Snake;
  friend class Food;

  void handleGameProcessing();

  /* --- Data members --- */
  bool holdFlag_{false};
  UserAction_t userAction_{Action};
  bool actionUsedFlag_{false};

  std::mutex timerMutex_;
  std::mutex gameMutex_;
  std::thread* timerThread_{nullptr};
  std::thread* gameThread_{nullptr};

  std::atomic<GameStatus_t> currentGameStatus_{START};

  GameInfo_t gameInfo_{};
  Snake* snake_{nullptr};
  Food* food_{nullptr};

  float gameTimer_{0.f};
};

}  // namespace s21

#endif  // SRC_SNAKE_H
