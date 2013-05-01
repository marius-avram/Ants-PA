#include "Logging.h"
#include "Bot.h"

int main() {
  std::cout.sync_with_stdio(0);

  Bot* bot = new Bot();
  bot->playGame();
  delete bot;

  return 0;
}
