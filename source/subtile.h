/*
 *  subtile.h
 *  deflektor-ds
 *
 *  Created by Hugh Cole-Baker on 1/19/09.
 *  A smaller tile (8x8) that is used for walls, etc
 */

#ifndef deflektor_subtile_h
#define deflektor_subtile_h

#include "beam.h"

enum Corner {
  TOPLEFT = 16,
  TOPRIGHT = 32,
  BOTTOMLEFT = 64,
  BOTTOMRIGHT = 128,
};

class Subtile {
protected:
  int bgHandle;
  unsigned int xPos;
  unsigned int yPos;
  const unsigned int tileBase;
  unsigned int palIdx;
  const unsigned int reflectiveSides;
  const bool goal;
  bool destroyed;
public:
  Subtile(unsigned int tile, unsigned int reflect, bool isGoal);
  unsigned int getReflectiveSides();
  void setBgHandle(int bg);
  void setXPos(unsigned int x);
  void setYPos(unsigned int y);
  void setPalIdx(unsigned int pal);
  virtual BeamResult beamEnters(unsigned int x, unsigned int y, BeamDirection angle);
  virtual void drawUpdate();
  bool isGoal();
  void destroy();
};

#endif
