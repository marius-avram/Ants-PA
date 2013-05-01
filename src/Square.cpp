#include "Square.h"

#include <iostream>

#include "global.h"

/** Resets each individual square after each turn. Only the water information
 * is persistent. */
void Square::reset() {
  isMarked = isFood = false;
  area = hillPlayer = -1;
  ant.missionType = NO_MISSION;
  ant.player = NO_PLAYER;
  nextSquare = 0;

  // At every new turn, this increases the probability of the square being
  // explored. (The invisibility rating is incremented).
  invisibility++;
}

/** Informs if the square is available to move an ant on. Used when deciding
 * on a move by a specific direction. */
bool Square::isFree() {
  return !isWater && ant.player == NO_PLAYER;
}

void Square::moveAntTo(Square* destination, MissionType missionType,
    bool dispersive) {
  if (!dispersive) {
    ant.player = NO_PLAYER;
  }
  ant.missionType = missionType;

  std::cout << "o" << " " << location.row << " " << location.column << " "
      << DIRECTION_LETTER[location.directionTo(destination->location)]
      << std::endl;

  // Update the destination square.
  destination->ant.player = ME;
  destination->ant.missionType = missionType;
}
