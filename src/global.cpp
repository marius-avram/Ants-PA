#include <stdint.h>
#include <iostream>
#include <fstream>

#ifndef LOGFILENAME
#define LOGFILENAME "logFile.log"
#endif

namespace gparam {
int mapRows;
int mapColumns;

int numberPlayers;

int64_t seed;

int totalTurnsNumber;

double attackRadius;
double spawnRadius;
double viewRadius;
int viewRange;

double loadTime;
double turnTime;

std::ofstream logFile(LOGFILENAME);
}
