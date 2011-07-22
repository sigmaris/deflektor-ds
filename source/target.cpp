/*
 *  target.cpp
 *  deflektor-ds
 *
 *  Created by Hugh Cole-Baker on 18/12/2008.
 *
 */
#include <nds.h>

#include "target.h"
#include "level.h"
#include "beam.h"

Target::Target(int bg, int x, int y, unsigned int palIdx, unsigned int tile, Level* level) : Tile(bg,x,y,palIdx), tileBase(tile), level(level), state(0)
{
  drawUpdate();
}

void Target::drawUpdate()
{
  volatile u16* mapPtr = (volatile u16*)bgGetMapPtr(bgHandle);
  if(state < 5)
  {
    mapPtr[pos2idx(xPos*2, yPos*2)] = ((paletteIndex << 12) | (tileBase+state*4));
    mapPtr[pos2idx(xPos*2+1, yPos*2)] = ((paletteIndex << 12) | (tileBase+1+state*4));
    mapPtr[pos2idx(xPos*2, yPos*2+1)] = ((paletteIndex << 12) | (tileBase+2+state*4));
    mapPtr[pos2idx(xPos*2+1, yPos*2+1)] = ((paletteIndex << 12) | (tileBase+3+state*4));
    if(state > 0)
    {
      if(frameCounter > 4)
      {
        state++;
        frameCounter = 0;
      }
      else
      {
        ++frameCounter;
      }
    }
  }
  else if(state == 5)
  {
    mapPtr[pos2idx(xPos*2, yPos*2)] = 0;
    mapPtr[pos2idx(xPos*2+1, yPos*2)] = 0;
    mapPtr[pos2idx(xPos*2, yPos*2+1)] = 0;
    mapPtr[pos2idx(xPos*2+1, yPos*2+1)] = 0;
    state++;
    //Notify the level that a target was destroyed
    level->targetDestroyed(this);
  }
}

BeamResult Target::beamEnters(unsigned int x, unsigned int y, BeamDirection atAngle)
{
  BeamResult res;
  if(x == 4*xPos+2 && y == 4*yPos+2 && state < 3) //entering center
  {
    res.action = Block;
    if(state == 0)
    {
      state = 1;
      //Notify the level that this was hit and it should start animating the explosion
      level->targetHit(this);
    }
  }
  else
  {
    res.action = Pass;
  }
  return res;
}
