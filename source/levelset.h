/*
 *  levelset.h
 *  deflektor-ds
 *
 *  Created by Hugh Cole-Baker on 4/1/09.
 *  A levelset manages a series of levels, and plays through them in sequence.
 *  There are also some functions for fading out the screen in between levels
 *  and if the player gets "game over"
 */

#ifndef deflektor_levelset_h
#define deflektor_levelset_h

#include <unistd.h>

struct LevelParams;
class Level;

class LevelSet {
  char levelStr[11];
  char livesStr[9];
  int currentScore, currentLives, currentLevel, dirLen;
public:
  static void fadeToBlack();
  static void fadeFromBlack();
  static void blackScreen();
  static void blackScreenSub();
  static void disableFade();
  static void fadeToBlackSub();
  static void disableFadeSub();
  void gameOver(Level* ended);
  static void captureScreen(int bank);
  static void blurScreen(int iters, int bank, bool fadeSub);
  bool playLevel(char* levelfile);

  char filename[MAXPATHLEN];

  LevelSet(char* directory);
  void playLevelSet();
};

#endif