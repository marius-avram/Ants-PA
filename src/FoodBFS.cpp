#include "FoodBFS.h"

#include "global.h"

int FoodBFS::nextStep() {
  if (squares.empty()) {
    // Completed unsuccessful search.
    return -1;
  }
  Square* current = squares.front();
  squares.pop();
  if (current->range > FOOD_RANGE) {
    // Completed unsuccessful search.
    return -1;
  }
  if (current->ant.player == ME && current->ant.missionType == NO_MISSION) {
    result = current;
    // Completed successful search.
    return 0;
  }
  for (std::list<Square*>::iterator it = current->neighbors.begin();
      it != current->neighbors.end(); ++it) {
    if ((*it)->isMarked) {
      continue;
    }
    (*it)->isMarked = true;
    (*it)->range = current->range + 1;
    (*it)->nextSquare = current;
    squares.push(*it);
    marked.push_back(*it);
  }
  // Search not finished.
  return 1;
}

/** Resets all squares marked in the performed searches. */
void FoodBFS::resetAll() {
  for (std::list<Square*>::iterator it = FoodBFS::marked.begin();
      it != FoodBFS::marked.end(); ++it) {
    (*it)->isMarked = false;
  }
  FoodBFS::marked.clear();
}

std::list<Square *> FoodBFS::marked;
