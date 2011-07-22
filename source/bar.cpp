/*
 *  bar.cpp
 *  deflektor-ds
 *
 *
 */

#include "bar.h"

Bar::Bar(coord pos, u16* gfxSource, int palIdx) :
AffineSprite(true, pos, 0, palIdx, SpriteSize_64x32, SpriteColorFormat_16Color, false, false, false, false, 1, true/*allocGfx*/, true, true),
value(0),
initX(pos.first)
{
  char * bottomBar = 64+((char*) gfxSource);
  for(int n = 0; n < 8; ++n)
  {
    dmaCopy(gfxSource, (n*32)+((char*)(gfxPtr[0])), 32);
    dmaCopy(bottomBar, 256+(n*32)+((char*)(gfxPtr[0])), 32);
  }
  dmaFillWords(0,512+((char*)(gfxPtr[0])),1024);
}

void Bar::setValue(unsigned int val)
{
  hidden = val <= 0;
  position.first = initX + (val >> 2);
  sx = divf32(1 << 12, val << 9);
  sy = 1 << 8;
  update();
}