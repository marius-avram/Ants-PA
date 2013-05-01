/** Structure for storing information about the current state of the map. */
#ifndef STATE_H_
#define STATE_H_

#include <iostream>
#include <vector>
#include <list>

#include "Square.h"
#include "Timer.h"

struct State {
  /** False while we keep playing. */
  bool gameOver;

  /** The current turn. */
  int currentTurnNumber;

  /** Score for each of the current players. */
  std::vector<double> scores;

  /** See definition of Square for further details. */
  std::vector<std::vector<Square> > grid;

  std::vector<Square*> myAnts;
  std::vector<Square*> enemyAnts;
  std::vector<Square*> myHills;
  std::vector<Square*> enemyHills;
  std::vector<Square*> food;

  /** Area number for each of myAnts */
  std::vector<int> myAreas;

  /** Border squares near each ant. */
  std::vector<std::list<Square*> > borders;

  /** This could have been global, but there you go... */
  Timer timer;

  /** Constructor creates the map proper. */
  State() :
      gameOver(false), currentTurnNumber(0) {
  }

  /** Clears non-persistent information from the grid after a step. */
  void reset();

  /** Marks visible cells. */
  void updateAreas();

  /** Marks visible squares on the grid. Determines myAreas and borders */
  void searchVisibleSquares();

  /** Returns the area at the location. */
  int getArea(const Square* s);
};

/** Method that helps do the IO. */
std::istream& operator>>(std::istream &is, State &state);

#endif // STATE_H_
