#include "tile.h"
#include <nds.h>

Tile::Tile(int bg, int x, int y, unsigned int palIdx) : bgHandle(bg), xPos(x), yPos(y), paletteIndex(palIdx)
{
}

Tile::~Tile()
{
  volatile u16* mapPtr = (volatile u16*)bgGetMapPtr(bgHandle);
  mapPtr[pos2idx(xPos*2, yPos*2)] = 0;
  mapPtr[pos2idx(xPos*2+1, yPos*2)] = 0;
  mapPtr[pos2idx(xPos*2, yPos*2+1)] = 0;
  mapPtr[pos2idx(xPos*2+1, yPos*2+1)] = 0;
}

void Tile::drawUpdate()
{
}

BeamResult Tile::beamEnters(unsigned int x, unsigned int y, BeamDirection angle)
{
  BeamResult res;
  res.action = Pass;
  return res;
}

bool Tile::isInteractive()
{
  return false;
}

void Tile::destroy()
{
  //do nothing
}

coord Tile::getPosition()
{
  return coord(xPos,yPos);
}