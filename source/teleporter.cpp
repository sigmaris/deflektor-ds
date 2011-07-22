/*
 *  teleporter.cpp
 *  deflektor-ds
 *
 *  Created by Hugh Cole-Baker on 1/18/09.
 *
 */

#include <nds.h>
#include "teleporter.h"

Teleporter::Teleporter(unsigned int bg, unsigned int x, unsigned int y, unsigned int palIdx, unsigned int tile) :
Tile(bg, x, y, palIdx),
tileBase(tile),
link(NULL),
destroyed(false)
{
  drawUpdate();
}

void Teleporter::setLink(Teleporter* toLink)
{
  link = toLink;
}

void Teleporter::drawUpdate()
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
    mapPtr[pos2idx(xPos*2, yPos*2)] = ((paletteIndex << 12) | tileBase);
    mapPtr[pos2idx(xPos*2+1, yPos*2)] = ((paletteIndex << 12) | (tileBase+1));
    mapPtr[pos2idx(xPos*2, yPos*2+1)] = ((paletteIndex << 12) | (tileBase+2));
    mapPtr[pos2idx(xPos*2+1, yPos*2+1)] = ((paletteIndex << 12) | (tileBase+3));
  }
}

BeamResult Teleporter::beamEnters(unsigned int x, unsigned int y, BeamDirection atAngle)
{
  BeamResult res;
  if(!destroyed && link && hitsCenter(x, y, atAngle))
  {
    res.action = Teleport;
    res.direction = atAngle;
    res.xPos = link->xPos*4 + 2;
    res.yPos = link->yPos*4 + 2;
  }
  else
  {
    res.action = Pass;
  }
  return res;
}

void Teleporter::destroy()
{
  if(!destroyed)
  {
    destroyed = true;
    drawUpdate();
    if(link) link->destroy();
  }
}
