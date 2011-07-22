/*
 *  spinblocker.h
 *  deflektor-ds
 *
 *  Created by Hugh Cole-Baker on 18/12/2008.
 *  A block that rotates, and only lets the beam through if it's aligned correctly.
 *  Otherwise the beam is blocked/absorbed
 */

#ifndef deflektor_spinblocker_h
#define deflektor_spinblocker_h

#define SPIN_BLOCKER_TILES 32

#include "tile.h"

class SpinBlocker : public Tile
{
  protected:
    const unsigned int tileBase;
    unsigned int angle;
    unsigned int frameCounter;
  public:
    SpinBlocker(int bg, int x, int y, unsigned int palIdx, unsigned int tile, BeamDirection initAngle);
    virtual void drawUpdate();
    virtual BeamResult beamEnters(unsigned int x, unsigned int y, BeamDirection atAngle);
};

#endif
