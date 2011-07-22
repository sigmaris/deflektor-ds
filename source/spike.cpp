/*
 *  spike.cpp
 *  deflektor-ds
 *
 *  Created by Hugh Cole-Baker on 17/12/2008.
 *
 */

#include "spike.h"

#include <nds.h>

Spike::Spike(int bg, int x, int y, unsigned int palIdx, unsigned int tile) : Tile(bg, x, y, palIdx), tileBase(tile), destroyed(false)
{
  drawUpdate();
}

void Spike::drawUpdate()
{
  volatile u16* mapPtr = (volatile u16*)bgGetMapPtr(bgHandle);
  if(!destroyed)
  {
    mapPtr[pos2idx(xPos*2, yPos*2)] = ((paletteIndex << 12) | tileBase);
    mapPtr[pos2idx(xPos*2+1, yPos*2)] = ((paletteIndex << 12) | (tileBase+1));
    mapPtr[pos2idx(xPos*2, yPos*2+1)] = ((paletteIndex << 12) | (tileBase+2));
    mapPtr[pos2idx(xPos*2+1, yPos*2+1)] = ((paletteIndex << 12) | (tileBase+3));
  }
  else
  {
    mapPtr[pos2idx(xPos*2, yPos*2)] = 0;
    mapPtr[pos2idx(xPos*2+1, yPos*2)] = 0;
    mapPtr[pos2idx(xPos*2, yPos*2+1)] = 0;
    mapPtr[pos2idx(xPos*2+1, yPos*2+1)] = 0;
  }
}

BeamResult Spike::beamEnters(unsigned int x, unsigned int y, BeamDirection atAngle)
{
  BeamResult res;
  if(!destroyed && hitsCenter(x,y,atAngle))
  {
    res.action = Overload;
    res.flag = 2;
  }
  else
  {
    res.action = Pass;
  }
  return res;
}

void Spike::destroy()
{
  destroyed = true;
  drawUpdate();
}