/*
 *  spinmirror.h
 *  deflektor-ds
 *
 *  Created by Hugh Cole-Baker on 10/01/2009.
 *  A mirror that is like a regular Mirror but constantly spins by itself.
 *
 */

#ifndef deflektor_spinmirror_h
#define deflektor_spinmirror_h

#include "mirror.h"

class SpinMirror : public Mirror
  {
  protected:
    unsigned int frameCounter;
  public:
    SpinMirror(int bg, int mirrorBg, int x, int y, unsigned int palIdx, unsigned int tileBase, BeamDirection initAngle);
    virtual void drawUpdate();
  };

#endif