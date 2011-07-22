/*
 *  spinblocker.cpp
 *  deflektor-ds
 *
 *  Created by Hugh Cole-Baker on 18/12/2008.
 *
 */

#include <nds.h>

#include "spinblocker.h"

SpinBlocker::SpinBlocker(int bg, int x, int y, unsigned int palIdx, unsigned int tile, BeamDirection initAngle) : Tile(bg, x, y, palIdx), tileBase(tile), angle(initAngle & 7)
{
  drawUpdate();
}

void SpinBlocker::drawUpdate()
{
  volatile u16* mapPtr = (volatile u16*)bgGetMapPtr(bgHandle);
  mapPtr[pos2idx(xPos*2, yPos*2)] = ((paletteIndex << 12) | (tileBase+angle*4));
  mapPtr[pos2idx(xPos*2+1, yPos*2)] = ((paletteIndex << 12) | (tileBase+1+angle*4));
  mapPtr[pos2idx(xPos*2, yPos*2+1)] = ((paletteIndex << 12) | (tileBase+2+angle*4));
  mapPtr[pos2idx(xPos*2+1, yPos*2+1)] = ((paletteIndex << 12) | (tileBase+3+angle*4));
  if(frameCounter > ANIMATION_DELAY)
  {
    angle++;
    if(angle & 8) angle = 0; 
    frameCounter = 0;
  }
  else
  {
    ++frameCounter;
  }
}

BeamResult SpinBlocker::beamEnters(unsigned int x, unsigned int y, BeamDirection atAngle)
{
  BeamResult res;
  if((atAngle & 7) == (BeamDirection)angle)
  {
    res.action = Pass;
  }
  else
  {
    res.action = Block;
  }
  return res;
}
