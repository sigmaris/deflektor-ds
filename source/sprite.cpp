/*
 *  sprite.cpp
 *  deflektor-ds
 *
 *  Created by Hugh Cole-Baker on 4/29/09.
 *
 */
#include <string.h>
#include <stdio.h>

#include "sprite.h"

//A bitmap of the 128 sprite slots
int Sprite::freeBitmap[4] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};

Sprite::Sprite(bool screen, coord pos, int prio, int palIdx,
               SpriteSize sz, SpriteColorFormat fmt,
               bool hide, bool hFlip, bool vFlip, bool mosaic, int frames, bool allocateGfx) :
               mainScreen(screen), position(pos), priority(prio), palIndex(palIdx),
               size(sz), colFmt(fmt), hidden(hide), flipH(hFlip), flipV(vFlip), 
               applyMosaic(mosaic), numFrames(frames), curFrame(0), managedGfx(allocateGfx)
{
  init();
}

Sprite::Sprite(bool screen, int palIdx, SpriteSize sz, SpriteColorFormat fmt, int frames) :
mainScreen(screen), position(coord(0,0)), priority(0), palIndex(palIdx),
size(sz), colFmt(fmt),
hidden(true), flipH(false), flipV(false), applyMosaic(false),
numFrames(frames), curFrame(0), managedGfx(false)
{
  init();
}

bool Sprite::init()
{
  unsigned int nextFree = 0;
  for (int i = 0; i < 4; ++i)
  {
    //Find first set bit
    nextFree = (freeBitmap[i] & -freeBitmap[i]);
    if(nextFree)
    {
      //Unset bit to mark it as in use
      freeBitmap[i] ^= nextFree;
      //Convert bit position to index
      nextFree = 32 - __builtin_clz(nextFree);
      //Multiply by position in freeBitmap
      nextFree += 32 * i;
      break;
    }
  }
  if(!nextFree)
  {
    //there are no free sprite slots.
    //If exceptions weren't disabled, we could throw one...
    iprintf("Out of sprite slots!\n");
    return false;
  }
  else
  {
    //Next free index found from bitmap is always off by +1
    --nextFree;
  }
  spriteIndex = nextFree;
  gfxPtr = new u16* [numFrames];
  if(managedGfx)
  {
    //Allocate VRAM for as many frames as we need
    for (int i = 0; i < numFrames; ++i)
    {
      gfxPtr[i] = oamAllocateGfx((mainScreen ? &oamMain : &oamSub), size, colFmt);
      if(!gfxPtr[i])
      {
        iprintf("Out of sprite VRAM!\n");
        return false; //there is no free sprite gfx mem
      }
    }
  }
  update();
  return true;
}

void Sprite::show()
{
  hidden = false;
  update();
}

void Sprite::hide()
{
  hidden = true;
  update();
}


void Sprite::setGfxPtrs(u16* gfxPtrs[])
{
  memcpy(gfxPtr, gfxPtrs, numFrames*sizeof(u16*));
}

u16** Sprite::getGfxPtrs()
{
  return gfxPtr;
}

void Sprite::setPosition(coord pos)
{
  position = pos;
  update();
}

void Sprite::setXPos(int xPos)
{
  position.first = xPos;
  update();
}

void Sprite::setYPos(int yPos)
{
  position.second = yPos;
  update();
}

coord& Sprite::getPosition()
{
  return position;
}

void Sprite::setAlphaBlend(bool alphaBlend)
{
  (mainScreen ? oamMain : oamSub).oamMemory[spriteIndex].blendMode = (alphaBlend ? OBJMODE_BLENDED : OBJMODE_NORMAL);
}

//Sets the values in OAM for this sprite, call after setting position, etc
void Sprite::update()
{
  oamSet((mainScreen ? &oamMain : &oamSub), spriteIndex, position.first, position.second, priority, palIndex, size, colFmt, gfxPtr[curFrame], -1, false, hidden, flipH, flipV, applyMosaic);
}

Sprite::~Sprite()
{
  oamSet((mainScreen ? &oamMain : &oamSub), spriteIndex, 0, 0, 0, 0, size, colFmt, gfxPtr[curFrame], -1, false, true, false, false, false);
  freeBitmap[spriteIndex >> 5] |= BIT(spriteIndex & 0x1F);
  if(managedGfx)
  {
    //Free our managed VRAM
    for (int i = 0; i < numFrames; ++i)
    {
      oamFreeGfx((mainScreen ? &oamMain : &oamSub), gfxPtr[i]);
    }
  }
  delete [] gfxPtr;
}