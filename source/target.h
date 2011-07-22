/*
 *  target.h
 *  deflektor-ds
 *
 *  Created by Hugh Cole-Baker on 18/12/2008.
 *
 */

#ifndef deflektor_target_h
#define deflektor_target_h

#include "tile.h"

class Level;

class Target : public Tile
{
protected:
  const unsigned int tileBase;
  Level* const level;
  unsigned int state;
  unsigned int frameCounter;
public:
  Target(int bg, int x, int y, unsigned int palIdx, unsigned int tile, Level* level);
  virtual void drawUpdate();
  virtual BeamResult beamEnters(unsigned int x, unsigned int y, BeamDirection atAngle);
};

#endif