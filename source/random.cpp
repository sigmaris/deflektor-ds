/*
 *  random.cpp
 *  deflektor-ds
 *
 *  Created by Hugh Cole-Baker on 1/17/09.
 *
 */

#include <nds.h>
#include "random.h"
#include "beam.h"
#include <stdlib.h>
#include <stdio.h>

Random::Random(int bg, int x, int y, unsigned int palIdx, unsigned int tile) :
Tile(bg,x,y,palIdx),
tileBase(tile),
randomFactor(mod32(rand(), 9) - 4)
{
  drawUpdate();
}

void Random::drawUpdate()
{
  volatile u16* mapPtr = (volatile u16*)bgGetMapPtr(bgHandle);
  
  mapPtr[pos2idx(xPos*2, yPos*2)] = ((paletteIndex << 12) | tileBase);
  mapPtr[pos2idx(xPos*2+1, yPos*2)] = ((paletteIndex << 12) | (tileBase+1));
  mapPtr[pos2idx(xPos*2, yPos*2+1)] = ((paletteIndex << 12) | (tileBase+2));
  mapPtr[pos2idx(xPos*2+1, yPos*2+1)] = ((paletteIndex << 12) | (tileBase+3));
}

BeamResult Random::beamEnters(unsigned int x, unsigned int y, BeamDirection atAngle)
{
  BeamResult res;
  if(hitsCenter(x, y, atAngle))
  {
    if(counter++ >= ANIMATION_DELAY)
    {
      counter = 0;
      randomFactor = mod32(rand(), 9) - 4;
    }
    int angle = (int)atAngle;
    
    res.direction = (BeamDirection)((angle + randomFactor) & 0xF);
    res.action = Teleport;
    coord nextPos(xPos * 4 + 2, yPos * 4 + 2);
    //Need to move the beam out of this block or it'll get randomized again
    do {
      res.xPos = nextPos.first;
      res.yPos = nextPos.second;
      nextPos = Beam::calculateNextPos(nextPos.first, nextPos.second, res.direction);
    } while(!(nextPos.first == xPos * 4 || nextPos.first == xPos * 4 + 4 || nextPos.second == yPos * 4 || nextPos.second == yPos * 4 + 4));
  }
  else
  {
    res.action = Pass;
  }
  
  return res;
}
