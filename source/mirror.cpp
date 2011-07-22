
#include <nds.h>
#include <stdlib.h>

#include "mirror.h"
#include "beam.h"

Mirror::Mirror(int bg, int mirrorBg, int x, int y, unsigned int palIdx, unsigned int tileBase) : Tile(bg, x, y, palIdx), mirrorTileBase(tileBase), mirrorBgHandle(mirrorBg), angle(rand() & 0xF)
{
  needsUpdate = true;
  drawUpdate();
}

Mirror::~Mirror()
{
  volatile u16* mirrorMapPtr = (volatile u16*)bgGetMapPtr(mirrorBgHandle);
  //clear the 4 base tiles on the map
  mirrorMapPtr[pos2idx(xPos*2, yPos*2)] = 0;
  mirrorMapPtr[pos2idx(xPos*2+1, yPos*2)] = 0;
  mirrorMapPtr[pos2idx(xPos*2, yPos*2+1)] = 0;
  mirrorMapPtr[pos2idx(xPos*2+1, yPos*2+1)] = 0;
}

void Mirror::rotate(int amount)
{
  angle += amount;
  angle &= 0xF;
  needsUpdate = true;
}

void Mirror::setAngle(int angleSet)
{
  angle = angleSet & 0xF;
  needsUpdate = true;
}

BeamResult Mirror::beamEnters(unsigned int x, unsigned int y, BeamDirection atAngle)
{
  BeamResult res;
  if(x == 4*xPos+2 && y == 4*yPos+2) //entering center
  {
    unsigned int incoming = (unsigned int)atAngle;
    unsigned int lineAngle = (incoming * 2) & 0xF;
    if(lineAngle == angle)
    {
      res.action = Pass;
    }
    else if(((lineAngle + 8) & 0xF) == angle)
    {
      res.action = Overload;
      res.flag = 1;
    }
    else
    {
      res.action = Reflect;
      res.xPos = x;
      res.yPos = y;
      res.direction = (BeamDirection)((incoming - (lineAngle - angle)) & 0xF);
    }
  }
  else
  {
    res.action = Pass;
  }
  return res;
}

void Mirror::drawUpdate()
{
  if(needsUpdate)
  {
    volatile u16* mapPtr = (volatile u16*)bgGetMapPtr(bgHandle);
    volatile u16* mirrorMapPtr = (volatile u16*)bgGetMapPtr(mirrorBgHandle);
    //fill in the 4 base tiles on the map
    mirrorMapPtr[pos2idx(xPos*2, yPos*2)] = ((paletteIndex << 12) | mirrorTileBase);
    mirrorMapPtr[pos2idx(xPos*2+1, yPos*2)] = ((paletteIndex << 12) | (mirrorTileBase+1));
    mirrorMapPtr[pos2idx(xPos*2, yPos*2+1)] = ((paletteIndex << 12) | (mirrorTileBase+2));
    mirrorMapPtr[pos2idx(xPos*2+1, yPos*2+1)] = ((paletteIndex << 12) | (mirrorTileBase+3));
    
    //fill in the 4 mirror-position tiles
    int angleTileIdx = 4+(4*angle);
    mapPtr[pos2idx(xPos*2,yPos*2)] = ((paletteIndex << 12) | (mirrorTileBase+angleTileIdx));
    mapPtr[pos2idx(xPos*2+1,yPos*2)] = ((paletteIndex << 12) | (mirrorTileBase+angleTileIdx+1));
    mapPtr[pos2idx(xPos*2,yPos*2+1)] = ((paletteIndex << 12) | (mirrorTileBase+angleTileIdx+2));
    mapPtr[pos2idx(xPos*2+1,yPos*2+1)] = ((paletteIndex << 12) | (mirrorTileBase+angleTileIdx+3));
    
    needsUpdate = false;
  }
}

bool Mirror::isInteractive()
{
  return true;
}