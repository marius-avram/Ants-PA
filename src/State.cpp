#include "State.h"

#include <iostream>
#include <cstdlib>
#include <string>
#include <stdint.h>
#include <iomanip>
#include <queue>
#include <cmath>

#include "global.h"
#include "Logging.h"

/** Resets all squares and clears non-persistent information about
 * areas, borders, ants and hills.
 */
void State::reset() {
  myAnts.clear();
  myHills.clear();
  enemyAnts.clear();
  enemyHills.clear();
  food.clear();
  myAreas.clear();
  borders.clear();

  // Reset every square individually.
  for (int row = 0; row < gparam::mapRows; row++) {
    for (int col = 0; col < gparam::mapColumns; col++) {
      if (!grid[row][col].isWater) {
        grid[row][col].reset();
      }
    }
  }
}

/** Get the global area number for a square. */
int State::getArea(const Square* s) {
  return myAreas[s->area];
}

/** Marks which squares on the map are visible.  The square which are
 * visible get their ratings reset to 0. After searching for visible squares
 * when the areas corresponding to two different ants are merged using the
 * adjacency list.
 */
void State::updateAreas() {
  myAreas.assign(myAnts.size(), -1);
  borders.assign(myAnts.size(), std::list<Square*>());
  searchVisibleSquares();

  // Areas corresponding to any two different ants which are adjacent are merged
  // using the adjacency list.
  for (int i = 0; i < (int) myAreas.size(); i++) {
    myAreas[i] = myAreas[myAreas[i]];
  }

  // Individual borders squares which prove to be visible by other ants are
  // removed from the state.
  for (int i = 0; i < (int) borders.size(); i++) {
    for (std::list<Square*>::iterator j = borders[i].begin();
        j != borders[i].end();) {
      if ((*j)->area != -1) {
        j = borders[i].erase(j);
        continue;
      }
      j++;
    }
  }
}

/** Using BSF from each ant, visible squares are marked, and border squares are
 * added to the state.
 */
void State::searchVisibleSquares() {
  std::queue<Square*, std::list<Square*> > squares;
  for (size_t i = 0; i < myAnts.size(); i++) {
    Square* antSquare = myAnts[i];

    antSquare->range = 0;
    antSquare->area = i;
    antSquare->invisibility = 0;
    squares.push(antSquare);
  }
  while (!squares.empty()) {
    Square* current = squares.front();
    squares.pop();

    for (std::list<Square*>::iterator j = current->neighbors.begin();
        j != current->neighbors.end(); ++j) {
      if ((*j)->area != -1) {
        // Already marked. Update areas adjacency list.
        if (current->area > (*j)->area) {
          myAreas[current->area] = (*j)->area;
        } else {
          myAreas[(*j)->area] = current->area;
        }
        continue;
      }
      if (current->range + 1 > gparam::viewRange
          && myAnts[current->area]->location.distanceTo((*j)->location)
              > gparam::viewRadius) {
        // Border square.
        borders[current->area].push_back(*j);
        continue;
      }
      (*j)->area = current->area;
      (*j)->range = current->range + 1;
      (*j)->invisibility = 0;
      squares.push(*j);
    }
  }
}

/** Input functions. */
std::istream& operator>>(std::istream &is, State &state) {
  int row, column, player;
  std::string inputType, junk;

  // Read in input type.
  while (is >> inputType) {
    if (inputType == "end") {
      state.gameOver = true;
      break;
    } else if (inputType == "turn") {
      is >> state.currentTurnNumber;
      break;
    } else {
      getline(is, junk);
    }
  }

  if (state.currentTurnNumber == 0) {
    // If we are at the beginning of the game, read in the parameters.
    while (is >> inputType) {
      if (inputType == "loadtime") {
        is >> gparam::loadTime;
      } else if (inputType == "turntime") {
        is >> gparam::turnTime;
      } else if (inputType == "rows") {
        is >> gparam::mapRows;
      } else if (inputType == "cols") {
        is >> gparam::mapColumns;
      } else if (inputType == "turns") {
        is >> gparam::totalTurnsNumber;
      } else if (inputType == "player_seed") {
        is >> gparam::seed;
        srand((unsigned int) gparam::seed);
      } else if (inputType == "viewradius2") {
        is >> gparam::viewRadius;
        // Compute the maximum Manhattan distance for which all the squares
        // around an ant are visible.
        gparam::viewRange = (int) std::floor(std::sqrt(gparam::viewRadius));
      } else if (inputType == "attackradius2") {
        is >> gparam::attackRadius;
      } else if (inputType == "spawnradius2") {
        is >> gparam::spawnRadius;
      } else if (inputType == "ready") {
        // This is the end of the parameter input.
        state.timer.start();
        break;
      } else {
        getline(is, junk);
      }
    }

    // Initialize empty grid.
    for (int i = 0; i < gparam::mapRows; i++) {
      state.grid.push_back(std::vector<Square>());
      for (int j = 0; j < gparam::mapColumns; j++) {
        state.grid[i].push_back(Square(Location(i, j)));
      }
    }

    // Link grid squares.
    for (int i = 0; i < gparam::mapRows; i++) {
      for (int j = 0; j < gparam::mapColumns; j++) {
        state.grid[i][j].neighbors.push_back(
            &state.grid[(i + ROW_DIRECTION[NORTH] + gparam::mapRows)
                % gparam::mapRows][j]);

        state.grid[i][j].neighbors.push_back(
            &state.grid[i][(j + COLUMN_DIRECTION[EAST] + gparam::mapColumns)
                % gparam::mapColumns]);

        state.grid[i][j].neighbors.push_back(
            &state.grid[(i + ROW_DIRECTION[SOUTH] + gparam::mapRows)
                % gparam::mapRows][j]);

        state.grid[i][j].neighbors.push_back(
            &state.grid[i][(j + COLUMN_DIRECTION[WEST] + gparam::mapColumns)
                % gparam::mapColumns]);
      }
    }
  } else {
    // Reads in information about the current turn.
    while (is >> inputType) {
      if (inputType == "w") {
        /* Water square. */
        is >> row >> column;
        state.grid[row][column].isWater = 1;
        // Breaking the connections with the water square.
        for (std::list<Square *>::iterator it = state.grid[row][column]
            .neighbors.begin(); it != state.grid[row][column].neighbors.end();
            ++it) {
          if ((*it)->isWater) {
            break;
          }
          for (std::list<Square *>::iterator itn = (*it)->neighbors.begin();
              itn != (*it)->neighbors.end(); ++itn) {
            if ((*itn)->location == state.grid[row][column].location) {
              (*it)->neighbors.erase(itn);
              break;
            }
          }
        }
      } else if (inputType == "f") {
        // Food square.
        is >> row >> column;
        state.grid[row][column].isFood = 1;
        state.food.push_back(&state.grid[row][column]);
      } else if (inputType == "a") {
        // Live ant square.
        is >> row >> column >> player;
        state.grid[row][column].ant.player = player;
        if (player == ME) {
          state.myAnts.push_back(&state.grid[row][column]);
        } else {
          state.enemyAnts.push_back(&state.grid[row][column]);
        }
      } else if (inputType == "d") {
        // Dead ant squares.
        is >> row >> column >> player;
      } else if (inputType == "h") {
        // Hill square.
        is >> row >> column >> player;
        state.grid[row][column].hillPlayer = player;
        if (player == ME) {
          state.myHills.push_back(&state.grid[row][column]);
        } else {
          state.enemyHills.push_back(&state.grid[row][column]);
        }
      } else if (inputType == "players") {
        // Information about the players.
        is >> gparam::numberPlayers;
      } else if (inputType == "scores") {
        // Information about the scores.
        state.scores = std::vector<double>(gparam::numberPlayers, 0.0);
        for (int p = 0; p < gparam::numberPlayers; p++) {
          is >> state.scores[p];
        }
      } else if (inputType == "go") {
        // Finished input.
        if (state.gameOver) {
          is.setstate(std::ios::failbit);
        } else {
          state.timer.start();
        }
        break;
      } else {
        getline(is, junk);
      }
    }
  }

  return is;
}
