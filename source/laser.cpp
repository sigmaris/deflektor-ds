/*
 *  laser.cpp
 *  deflektor-ds
 *
 *  Created by Hugh Cole-Baker on 14/12/2008.
 *
 */

#include <nds.h>

#include "laser.h"

Laser::Laser(int bg, int x, int y, unsigned int palIdx, unsigned int tileBase, LaserDirection direction) : Tile(bg, x, y, palIdx), laserTileBase(tileBase), dir(direction)
{
  drawUpdate();
}

void Laser::drawUpdate()
{
  volatile u16* mapPtr = (volatile u16*)bgGetMapPtr(bgHandle);
  switch (dir) {
    case UP:
      //facing up
      mapPtr[pos2idx(xPos*2, yPos*2)] = ((paletteIndex << 12) | laserTileBase);
      mapPtr[pos2idx(xPos*2+1, yPos*2)] = ((paletteIndex << 12) | (laserTileBase+1));
      mapPtr[pos2idx(xPos*2, yPos*2+1)] = ((paletteIndex << 12) | (laserTileBase+2));
      mapPtr[pos2idx(xPos*2+1, yPos*2+1)] = ((paletteIndex << 12) | (laserTileBase+3));
      break;
    case RIGHT:
      //facing right
      mapPtr[pos2idx(xPos*2, yPos*2)] = ((paletteIndex << 12) | (laserTileBase+4));
      mapPtr[pos2idx(xPos*2+1, yPos*2)] = ((paletteIndex << 12) | (laserTileBase+5));
      mapPtr[pos2idx(xPos*2, yPos*2+1)] = ((paletteIndex << 12) | (laserTileBase+6));
      mapPtr[pos2idx(xPos*2+1, yPos*2+1)] = ((paletteIndex << 12) | (laserTileBase+7));
      break;
    case DOWN:
      //facing down - facing up mirrored
      mapPtr[pos2idx(xPos*2, yPos*2)] = ((paletteIndex << 12) | (laserTileBase+2) | BIT(11));
      mapPtr[pos2idx(xPos*2+1, yPos*2)] = ((paletteIndex << 12) | (laserTileBase+3) | BIT(11));
      mapPtr[pos2idx(xPos*2, yPos*2+1)] = ((paletteIndex << 12) | laserTileBase | BIT(11));
      mapPtr[pos2idx(xPos*2+1, yPos*2+1)] = ((paletteIndex << 12) | (laserTileBase+1) | BIT(11));
      break;
    case LEFT:
      //facing left - facing right mirrored
      mapPtr[pos2idx(xPos*2, yPos*2)] = ((paletteIndex << 12) | (laserTileBase+5) | BIT(10));
      mapPtr[pos2idx(xPos*2+1, yPos*2)] = ((paletteIndex << 12) | (laserTileBase+4) | BIT(10));
      mapPtr[pos2idx(xPos*2, yPos*2+1)] = ((paletteIndex << 12) | (laserTileBase+7) | BIT(10));
      mapPtr[pos2idx(xPos*2+1, yPos*2+1)] = ((paletteIndex << 12) | (laserTileBase+6) | BIT(10));
      break;
    default:
      break;
  }
}

BeamResult Laser::beamEnters(unsigned int x, unsigned int y, BeamDirection atAngle)
{
  BeamResult res;
  if(hitsCenter(x,y,atAngle))
  {
    res.action = Block;
  }
  else
  {
    res.action = Pass;
  }
  return res;
}

BeamResult Laser::getBeamStart()
{
  BeamResult res;
  res.action = Teleport;
  switch (dir)
  {
    case UP:
      res.xPos = 4*xPos+2;
      res.yPos = 4*yPos;
      res.direction = N;
      break;
    case RIGHT:
      res.xPos = 4*xPos+4;
      res.yPos = 4*yPos+2;
      res.direction = E;
      break;
    case DOWN:
      res.xPos = 4*xPos+2;
      res.yPos = 4*yPos+4;
      res.direction = S;
      break;
    case LEFT:
      res.xPos = 4*xPos;
      res.yPos = 4*yPos+2;
      res.direction = W;
      break;
    default:
      res.direction = SE;
      break;
  }
  return res;
}
