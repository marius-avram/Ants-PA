/** Structure which represents a location on the grid. */
#ifndef LOCATION_H_
#define LOCATION_H_

/** Structure for representing a location on the map. */
struct Location {
  int row;
  int column;

  Location(int row, int column) :
      row(row), column(column) {
  }

  /** Returns the location resulting from moving in the given direction. */
  Location move(int dir);

  /** Computes the direction towards a neighboring square. */
  int directionTo(const Location& l);

  /** Computes the distance to the given location. */
  int distanceTo(const Location &other);

  bool operator==(const Location &other);
};

#endif // LOCATION_H_
