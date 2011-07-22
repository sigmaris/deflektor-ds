/*
 *  receiver.cpp
 *  deflektor-ds
 *
 *  Created by Hugh Cole-Baker on 14/12/2008.
 *
 */

#include <nds.h>

#include "receiver.h"

Receiver::Receiver(int bg, int x, int y, unsigned int palIdx, unsigned int tileBase, LaserDirection direction) : Tile(bg, x, y, palIdx), receiverTileBase(tileBase+8), dir(direction)
{
  drawUpdate();
}

void Receiver::drawUpdate()
{
  volatile u16* mapPtr = (volatile u16*)bgGetMapPtr(bgHandle);
  switch (dir) {
    case UP:
      //facing up
      mapPtr[pos2idx(xPos*2, yPos*2)] = ((paletteIndex << 12) | receiverTileBase);
      mapPtr[pos2idx(xPos*2+1, yPos*2)] = ((paletteIndex << 12) | (receiverTileBase+1));
      mapPtr[pos2idx(xPos*2, yPos*2+1)] = ((paletteIndex << 12) | (receiverTileBase+2));
      mapPtr[pos2idx(xPos*2+1, yPos*2+1)] = ((paletteIndex << 12) | (receiverTileBase+3));
      break;
    case RIGHT:
      //facing right
      mapPtr[pos2idx(xPos*2, yPos*2)] = ((paletteIndex << 12) | (receiverTileBase+4));
      mapPtr[pos2idx(xPos*2+1, yPos*2)] = ((paletteIndex << 12) | (receiverTileBase+5));
      mapPtr[pos2idx(xPos*2, yPos*2+1)] = ((paletteIndex << 12) | (receiverTileBase+6));
      mapPtr[pos2idx(xPos*2+1, yPos*2+1)] = ((paletteIndex << 12) | (receiverTileBase+7));
      break;
    case DOWN:
      //facing down - facing up mirrored
      mapPtr[pos2idx(xPos*2, yPos*2)] = ((paletteIndex << 12) | (receiverTileBase+2) | BIT(11));
      mapPtr[pos2idx(xPos*2+1, yPos*2)] = ((paletteIndex << 12) | (receiverTileBase+3) | BIT(11));
      mapPtr[pos2idx(xPos*2, yPos*2+1)] = ((paletteIndex << 12) | receiverTileBase | BIT(11));
      mapPtr[pos2idx(xPos*2+1, yPos*2+1)] = ((paletteIndex << 12) | (receiverTileBase+1) | BIT(11));
      break;
    case LEFT:
      //facing left - facing right mirrored
      mapPtr[pos2idx(xPos*2, yPos*2)] = ((paletteIndex << 12) | (receiverTileBase+5) | BIT(10));
      mapPtr[pos2idx(xPos*2+1, yPos*2)] = ((paletteIndex << 12) | (receiverTileBase+4) | BIT(10));
      mapPtr[pos2idx(xPos*2, yPos*2+1)] = ((paletteIndex << 12) | (receiverTileBase+7) | BIT(10));
      mapPtr[pos2idx(xPos*2+1, yPos*2+1)] = ((paletteIndex << 12) | (receiverTileBase+6) | BIT(10));
      break;
    default:
      break;
  }
}

BeamResult Receiver::beamEnters(unsigned int x, unsigned int y, BeamDirection atAngle)
{
  BeamResult res;
  unsigned int relX = x - xPos * 4;
  unsigned int relY = y - yPos * 4;
  if(hitsCenter(x,y,atAngle))
  {
    res.action = Block;
    switch(dir)
    {
      case UP:
        if(atAngle == S && relX == 2)
          res.action = Complete;
        break;
      case DOWN:
        if(atAngle == N && relX == 2)
          res.action = Complete;
        break;
      case LEFT:
        if(atAngle == E && relY == 2)
          res.action = Complete;
        break;
      case RIGHT:
        if(atAngle == W && relY == 2)
          res.action = Complete;
        break;
    }
  }
  else
  {
    res.action = Pass;
  }
  return res;
}
