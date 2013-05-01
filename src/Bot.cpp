#include "Bot.h"

#include <cmath>
#include <cstdlib>

#include "FoodBFS.h"
#include "Logging.h"

void Bot::playGame() {
  /* Reads in game parameters. */
  std::cin >> state;
  endTurn();

  srand((unsigned int) gparam::seed);

  /* Continues to make moves until game is over. */
  while (std::cin >> state) {
    state.updateAreas();
    makeMoves();
    endTurn();
  }
}

void Bot::kamikaze() {
  static Square* target = NULL;
  if (target == NULL) {
    if (state.enemyHills.empty()) {
      return;
    }
    target = state.enemyHills[0];
  } else {
    if (target->ant.player != NO_PLAYER
        && target->hillPlayer != target->ant.player) {
      target = NULL;
      kamikaze();
      return;
    }
  }

  // Start BFS.
  std::list<Square*> marked;
  std::queue<Square*, std::list<Square*> > squares;
  squares.push(target);
  while (!squares.empty()) {
    Square* s = squares.front();
    squares.pop();
    for (std::list<Square*>::iterator i = s->neighbors.begin();
        i != s->neighbors.end(); ++i) {
      if ((*i)->isMarked) {
        continue;
      }

      // Check if an ant suitable for kamikaze mission has been found.
      if ((*i)->ant.player == ME) {
        if ((*i)->ant.missionType == NO_MISSION) {
          if (s->ant.player != ME) {
            // Move the ant to the square from which this one was reached.
            (*i)->moveAntTo(s, KAMIKAZE, false);
          } else {
            // Ignore ant if not possible to perform the move.
            // Wait for the square to be reached from another direction.
            continue;
          }
        }
      }
      (*i)->isMarked = true;
      marked.push_back((*i));
      squares.push((*i));
    }
  }
  for (std::list<Square*>::iterator i = marked.begin(); i != marked.end();
      ++i) {
    (*i)->isMarked = false;
  }
}

void Bot::exploreNear() {
  for (size_t j = 0; j < state.myAnts.size(); j++) {
    Square* antSquare = state.myAnts[j];
    Location antLocation = antSquare->location;
    std::list<Square*>& currentBorders = state.borders[j];

    // Check if the current ant is free and near the border.
    if (state.myAnts[j]->ant.missionType != NO_MISSION
        || currentBorders.size() == 0) {
      continue;
    }

    // Starting BFS from the border to compute the sum of the invisibility
    // ratings in the vicinity.
    std::list<Square*> marked;
    std::queue<Square*, std::list<Square*> > squares;

    for (std::list<Square*>::iterator i = currentBorders.begin();
        i != currentBorders.end(); ++i) {
      (*i)->isMarked = true;
      (*i)->range = 0;
      marked.push_back(*i);
      squares.push(*i);
    }

    int directionWeightRow = 0;
    int directioWeightColumn = 0;

    while (!squares.empty()) {
      Square* current = squares.front();
      squares.pop();

      // Computing row and column distances and directions.
      int dRow = current->location.row - antLocation.row;
      int dRowSign;
      if (dRow < 0) {
        dRow = -dRow;
        dRowSign = -1;
      } else {
        dRowSign = 1;
      }
      int dRowTemp = gparam::mapRows - dRow;
      if (dRowTemp < dRow) {
        dRow = dRowTemp;
        dRowSign = -dRowSign;
      }

      int dColumn = current->location.column - antLocation.column;
      int dColumnSign;
      if (dColumn < 0) {
        dColumn = -dColumn;
        dColumnSign = -1;
      } else {
        dColumnSign = 1;
      }
      int dColumnTemp = gparam::mapColumns - dColumn;
      if (dColumnTemp < dColumn) {
        dColumn = dColumnTemp;
        dColumnSign = -dColumnSign;
      }

      // Add vectors to the final sum.
      directionWeightRow += dRowSign * dRow * current->invisibility;
      directioWeightColumn += dColumnSign * dColumn * current->invisibility;

      if (current->range < BORDER_RANGE) {
        for (std::list<Square*>::iterator i = current->neighbors.begin();
            i != current->neighbors.end(); ++i) {
          if ((*i)->isMarked || (*i)->invisibility == 0) {
            continue;
          }
          (*i)->isMarked = true;
          (*i)->range = current->range + 1;
          marked.push_back(*i);
          squares.push(*i);
        }
      }
    }

    int direction, directionBackup;
    if (std::abs(directionWeightRow) > std::abs(directioWeightColumn)) {
      direction = directionWeightRow < 0 ? NORTH : SOUTH;
      directionBackup = directioWeightColumn < 0 ? WEST : EAST;
    } else {
      direction = directioWeightColumn < 0 ? WEST : EAST;
      directionBackup = directionWeightRow < 0 ? NORTH : SOUTH;
    }

    Location location = antLocation.move(direction);
    Location locationBackup = antLocation.move(directionBackup);
    if (!state.grid[location.row][location.column].isFree()) {
      location = locationBackup;
      direction = directionBackup;
    }
    if (state.grid[location.row][location.column].isFree()) {

      antSquare->moveAntTo(&state.grid[location.row][location.column],
          EXPLORE_NEAR, false);
    }

    for (std::list<Square*>::iterator i = marked.begin(); i != marked.end();
        ++i) {
      (*i)->isMarked = false;
    }
  }
}

void Bot::makeMoves() {
  grabFood();
  exploreNear();
  kamikaze();

  for (size_t i = 0; i < state.myAnts.size(); ++i) {
    Square* s = state.myAnts[i];
    if (s->ant.missionType != NO_MISSION || s->ant.player != ME) {
      continue;
    }
    int direction = rand() % s->neighbors.size();
    std::list<Square*>::iterator k;
    for (k = s->neighbors.begin(); direction > 0; direction--) {
      k++;
    }

    if ((*k)->ant.player == NO_PLAYER) {
      s->moveAntTo((*k), RANDOM, true);
    }
  }
}

void Bot::endTurn() {

  // If this wasn't the start game, reset the board.
  if (state.currentTurnNumber > 0) {
    state.reset();
  }

  // Move to next turn.
  state.currentTurnNumber++;

  std::cout << "go" << std::endl;
}

void Bot::grabFood() {
  // Start BFS' for each visible food.
  std::list<FoodBFS> searches;
  for (size_t i = 0; i < state.food.size(); i++) {
    searches.push_back(FoodBFS(state.food[i]));
  }

  bool areActiveSearches = true;
  // Perform one step in each search while they are reporting an unfinished
  // status.
  while (areActiveSearches) {
    areActiveSearches = false;
    for (std::list<FoodBFS>::iterator it = searches.begin();
        it != searches.end();) {
      int result = (*it).nextStep();
      if (result == 1) {
        areActiveSearches = true;
        it++;
        continue;
      }
      if (result == 0) {
        // An ant has been located.
        Square* ant = (*it).result;
        ant->moveAntTo(ant->nextSquare, FOOD, false);
      }
      it = searches.erase(it);
    }
  }
  FoodBFS::resetAll();
}
