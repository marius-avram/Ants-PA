#include "Location.h"

#include <cmath>

#include "global.h"

/** Returns the location resulting in a move in the direction supplied. */
Location Location::move(int dir) {
  Location returnValue(row + ROW_DIRECTION[dir],
      column + COLUMN_DIRECTION[dir]);

  if (returnValue.row < 0) {
    returnValue.row += gparam::mapRows;
  } else if (returnValue.row == gparam::mapRows) {
    returnValue.row = 0;
  }

  if (returnValue.column < 0) {
    returnValue.column += gparam::mapColumns;
  } else if (returnValue.column == gparam::mapColumns) {
    returnValue.column = 0;
  }

  return returnValue;
}

/* Returns the square of euclidean distance between two locations with the edges
 * wrapped.
 */
int Location::distanceTo(const Location& other) {
  int row = this->row - other.row;
  row = row < 0 ? -row : row;

  int column = this->column - other.column;
  column = column < 0 ? -column : column;

  row = row < (gparam::mapRows - row) ? row : (gparam::mapRows - row);
  column =
      column < (gparam::mapColumns - column) ? column :
          (gparam::mapColumns - column);

  return row * row + column * column;
}

/** Returns the direction to a neighboring location. */
int Location::directionTo(const Location& l) {
  int direction = l.row - row;
  int directionIndex = 1;
  if (direction == 0) {
    direction = l.column - column;
    directionIndex = -2;
  }
  if (direction < -1) {
    direction = 1;
  } else if (direction > 1) {
    direction = -1;
  }
  return std::abs(direction + directionIndex);
}

bool Location::operator==(const Location &other) {
  return row == other.row && column == other.column;
}
