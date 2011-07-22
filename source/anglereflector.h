/*
 *  anglereflector.h
 *  deflektor-ds
 *
 *  Created by Hugh Cole-Baker on 1/18/09.
 *  Reflects the beam unless it enters the block at a certain angle.
 *
 */


#ifndef deflektor_anglereflector_h
#define deflektor_anglereflector_h

#include "spinreflector.h"

class AngleReflector : public SpinReflector
{
  protected:
    bool destroyed;
  public:
    AngleReflector(int bg, int x, int y, unsigned int palIdx, unsigned int tile, BeamDirection setAngle, int reflectiveSides);
    virtual void drawUpdate();
    virtual BeamResult beamEnters(unsigned int x, unsigned int y, BeamDirection atAngle);
    virtual void destroy();
};

#endif
