/*
 *  star.cpp
 *  deflektor-ds
 *
 *  Created by Hugh Cole-Baker on 5/16/09.
 *
 */

#include "star.h"
#include "settings.h"

Star::Star(int palIdx) : Sprite(true, palIdx, SpriteSize_8x8, SpriteColorFormat_16Color, 1),
dx(0),
dy(0),
fx(512),
fy(384)
{
  setGfxPtrs((u16*[]){Settings::get().getSpriteGfx().starGfx});
  if(rand() & 1) flipH = true;
  if(rand() & 1) flipV = true;
  setPosition(coord(fx>>2,fy>>2));
  hide();
}

void Star::drawUpdate()
{
  fx += dx;
  fy += dy;
  if(hidden && (rand() & 15) == 15)
  {
    dx = mod32(rand(), 9) - 4;
    dy = mod32(rand(), 9) - 4;
    if(dx == 0 && dy == 0)
    {
      dx = 1;
      dy = 1;
    }
    show();
  }
  if(fx < 0 || fy < 0 || fx > 1024 || fy > 768)
  {
    fx = 512;
    fy = 384;
    dx = mod32(rand(), 9) - 4;
    dy = mod32(rand(), 9) - 4;
    if(dx == 0 && dy == 0)
    {
      dx = 1;
      dy = 1;
    }
    show();
  }
  setPosition(coord(fx>>2,fy>>2));
  update();
}