/** Structure for representing a square in the grid. */
#ifndef SQUARE_H_
#define SQUARE_H_

#include <list>

#include "Location.h"
#include "Ant.h"

struct Square {
  Location location;

  bool isWater;
  bool isFood;

  /** Square is marked in a search. */
  bool isMarked;

  Ant ant;
  int hillPlayer;
  int range;
  int invisibility;
  int area;
  std::list<Square*> neighbors;

  /** Next square in a path. */
  Square* nextSquare;

  Square(Location location) :
      location(location), isWater(false), isFood(false), isMarked(false),
          hillPlayer(NO_PLAYER), range(0), invisibility(0), area(-1),
          nextSquare(0) {
  }

  /** An ant can be moved to this square. */
  bool isFree();

  /** Resets the information for the square except water information. */
  void reset();

  /** Moves an ant from this square to another. They have to be neighbors. */
  void moveAntTo(Square* destination, MissionType missionType, bool dispersive);
};

#endif // SQUARE_H_
