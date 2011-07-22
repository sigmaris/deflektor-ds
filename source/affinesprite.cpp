/*
 *  affinesprite.cpp
 *  deflektor-ds
 *
 *
 */

#include "affinesprite.h"
#include <stdio.h>

//Bitmap of the 32 possible affine transform slots in OAM
int AffineSprite::freeBitmap = 0xFFFFFFFF;

bool AffineSprite::init()
{
  if(allocateAffine)
  {
    unsigned int nextFree = 0;
    nextFree = (freeBitmap & -freeBitmap);
    if(nextFree)
    {
      freeBitmap ^= nextFree;
      nextFree = 31 - __builtin_clz(nextFree);
    }
    else
    {
      //there are no free affine slots.
      iprintf("No free affine data slots!\n");
      return false;
    }
    affineIndex = nextFree;
  }
  update();
  return true;
}

AffineSprite::AffineSprite(bool screen, int palIdx, SpriteSize sz, SpriteColorFormat fmt,
                           int frames, bool allocAffine) :
Sprite(screen, palIdx, sz, fmt, frames), allocateAffine(allocAffine), doubleSize(false)
{
  init();
}

AffineSprite::AffineSprite(bool screen, coord pos, int prio, int palIdx,
                           SpriteSize sz, SpriteColorFormat fmt,
                           bool hide, bool hFlip, bool vFlip, bool mosaic, int frames,
                           bool allocateGfx, bool allocAffine, bool doubleSz) : 
Sprite(screen, pos, prio, palIdx, sz, fmt, hide, hFlip, vFlip, mosaic, frames, allocateGfx),
allocateAffine(allocAffine),
doubleSize(doubleSz)
{
  init();
}

AffineSprite::~AffineSprite()
{
  if(allocateAffine)
  {
    freeBitmap |= BIT(affineIndex);
  }
}

void AffineSprite::update()
{
  oamRotateScale((mainScreen ? &oamMain : &oamSub), affineIndex, angle, sx, sy);
  oamSet((mainScreen ? &oamMain : &oamSub), spriteIndex, position.first, position.second, priority, palIndex, size, colFmt, gfxPtr[curFrame], affineIndex, doubleSize, hidden, flipH, flipV, applyMosaic);
}
