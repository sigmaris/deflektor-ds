/*
 *  anglereflector.cpp
 *  deflektor-ds
 *
 *  Created by Hugh Cole-Baker on 1/18/09.
 *
 */

#include <nds.h>
#include "anglereflector.h"

AngleReflector::AngleReflector(int bg, int x, int y, unsigned int palIdx, unsigned int tile, BeamDirection setAngle, int reflectiveSides) : 
SpinReflector(bg, x, y, palIdx, tile, setAngle, reflectiveSides),
destroyed(false)
{
  angle = (setAngle & 7);
  drawUpdate();
}

void AngleReflector::drawUpdate()
{
  volatile u16* mapPtr = (volatile u16*)bgGetMapPtr(bgHandle);
  if(destroyed)
  {
    mapPtr[pos2idx(xPos*2, yPos*2)] = 0;
    mapPtr[pos2idx(xPos*2+1, yPos*2)] = 0;
    mapPtr[pos2idx(xPos*2, yPos*2+1)] = 0;
    mapPtr[pos2idx(xPos*2+1, yPos*2+1)] = 0;
  }
  else
  {
    mapPtr[pos2idx(xPos*2, yPos*2)] = ((paletteIndex << 12) | (tileBase+angle*4));
    mapPtr[pos2idx(xPos*2+1, yPos*2)] = ((paletteIndex << 12) | (tileBase+1+angle*4));
    mapPtr[pos2idx(xPos*2, yPos*2+1)] = ((paletteIndex << 12) | (tileBase+2+angle*4));
    mapPtr[pos2idx(xPos*2+1, yPos*2+1)] = ((paletteIndex << 12) | (tileBase+3+angle*4));
  }
}

BeamResult AngleReflector::beamEnters(unsigned int x, unsigned int y, BeamDirection atAngle)
{
  BeamResult res;
  if(destroyed)
  {
    res.action = Pass;
  }
  else
  {
    res = SpinReflector::beamEnters(x, y, atAngle);
  }
  return res;
}

void AngleReflector::destroy()
{
  destroyed = true;
  drawUpdate();
}