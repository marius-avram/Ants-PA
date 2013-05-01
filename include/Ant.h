/** Structure which represents an ant at a specific moment in the game. */
#ifndef ANT_H_
#define ANT_H_

/** Possible missions for an ant. */
enum MissionType {
  NO_MISSION, FOOD, EXPLORE_NEAR, EXPLORE_FAR, KAMIKAZE, RANDOM
};

/** Constants representing ant owners. */
enum Player {
  NO_PLAYER = -1, ME = 0
};

struct Ant {
  /** The current mission of the ant. */
  MissionType missionType;

  /** The player to which the ant belongs to. */
  int player;

  Ant() :
      missionType(NO_MISSION), player(NO_PLAYER) {
  }

  Ant(int player) :
      missionType(NO_MISSION), player(player) {
  }
};

#endif // ANT_H_
