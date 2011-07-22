/*
 *  spinreflector.h
 *  deflektor-ds
 *
 *  Created by Hugh Cole-Baker on 18/12/2008.
 *  A block that rotates, and only lets the beam through if it's aligned correctly.
 *  Otherwise the beam is reflected.
 */

#ifndef deflektor_spinreflector_h
#define deflektor_spinreflector_h

#include "spinblocker.h"

class SpinReflector : public SpinBlocker
{
  protected:
    int reflectiveSides;
  public:
    SpinReflector(int bg, int x, int y, unsigned int palIdx, unsigned int tile, BeamDirection initAngle, int ref);
    virtual BeamResult beamEnters(unsigned int x, unsigned int y, BeamDirection atAngle);
};

#endif
