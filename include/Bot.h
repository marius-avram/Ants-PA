/** Structure that represents an AI in a game. */
#ifndef BOT_H_
#define BOT_H_

#include "State.h"

struct Bot {
  /** Current state of the game. */
  State state;

  /** Plays a single game of Ants. */
  void playGame();

  /** Moves ants on the board. */
  void makeMoves();

  /** Find closest ant for every food tile and move the ant in that
   * direction. */
  void grabFood();

  /** Indicates to the engine that it has made its moves. */
  void endTurn();

  /** Sends ants which are near the border to explore. */
  void exploreNear();

  /** Move idle ants towards the enemy hill. */
  void kamikaze();
};

#endif
