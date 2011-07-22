/*
 *  laser.h
 *  deflektor-ds
 *
 *  Created by Hugh Cole-Baker on 14/12/2008.
 *  This is the source of the beam on the game board
 *
 */

#ifndef deflektor_laser_h
#define deflektor_laser_h

#include "tile.h"

typedef enum {
  UP = 1,
  DOWN = 2,
  LEFT = 4,
  RIGHT = 8
} LaserDirection;

class Laser : public Tile
{
protected:
  unsigned int laserTileBase;
  LaserDirection dir;
public:
  Laser(int bg, int x, int y, unsigned int palIdx, unsigned int tileBase, LaserDirection direction);
  virtual void drawUpdate();
  virtual BeamResult beamEnters(unsigned int x, unsigned int y, BeamDirection atAngle);
  BeamResult getBeamStart();
};

#endif