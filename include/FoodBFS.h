/** Structure for information about the breadth first search for the ants for
 * the current food tile */
#ifndef FOODBFS_H_
#define FOODBFS_H_

#include <queue>
#include <list>

#include "Square.h"

struct FoodBFS {
  /** The queue for the BFS. */
  std::queue<Square*, std::list<Square *> > squares;

  /** The list of marked squares, which will need to be reseted. */
  static std::list<Square*> marked;

  /** The starting point of the search. */
  Square* food;

  /** The Square on which an ant is found, and the next step the ant should
   ** make. */
  Square* result;

  FoodBFS(Square* food) :
      food(food) {
    food->isMarked = true;
    food->range = 0;
    squares.push(food);
    marked.push_back(food);
  }

  /** Analyzes the next square in the queue. */
  int nextStep();

  /** Resets the marked squares. */
  static void resetAll();
};

#endif
