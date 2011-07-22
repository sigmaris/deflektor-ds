/*
 *  random.h
 *  deflektor-ds
 *
 *  Created by Hugh Cole-Baker on 1/17/09.
 *  If the beam enters the center of this block it'll emerge in a random direction.
 *
 */

#ifndef deflektor_random_h
#define deflektor_random_h

#include "tile.h"

class Random : public Tile
  {
  protected:
    const unsigned int tileBase;
    unsigned int counter;
    int randomFactor;
  public:
    Random(int bg, int x, int y, unsigned int palIdx, unsigned int tile);
    virtual void drawUpdate();
    virtual BeamResult beamEnters(unsigned int x, unsigned int y, BeamDirection atAngle);
  };

#endif