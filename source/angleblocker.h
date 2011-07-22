/*
 *  angleblocker.h
 *  deflektor-ds
 *
 *  Created by Hugh Cole-Baker on 1/18/09.
 *  Blocks the beam unless it enters at a certain angle.
 *
 */

#ifndef deflektor_angleblocker_h
#define deflektor_angleblocker_h

#include "spinblocker.h"

class AngleBlocker : public SpinBlocker
{
  protected:
    bool destroyed;
  public:
    AngleBlocker(int bg, int x, int y, unsigned int palIdx, unsigned int tile, BeamDirection setAngle);
    virtual void drawUpdate();
    virtual BeamResult beamEnters(unsigned int x, unsigned int y, BeamDirection atAngle);
    virtual void destroy();
};

#endif
