/*
 *  gremlin.cpp
 *  deflektor-ds
 *
 *  Created by Hugh Cole-Baker on 4/4/09.
 *
 */

#include <nds.h>
#include <stdlib.h>
#include <stdio.h>
#include "gremlin.h"
#include "settings.h"
#include "tile.h"
#include "mirror.h"
#include "level.h"

Gremlin::Gremlin(Level* lev) : Sprite(true, 4, SpriteSize_16x16, SpriteColorFormat_16Color, 2),
taps(0),
frameCounter(0),
x(16*(mod32(rand(), 14))),
y(16*(mod32(rand(), 9))),
dx(0),
dy(0),
state(Paused),
associatedLevel(lev)
{
  iprintf("constructing gremlin %p\n",this);
  int type = rand() & 3;
  SpriteGfx& gfx = Settings::get().getSpriteGfx();
  switch (type) {
    case 0:
      setGfxPtrs((u16*[]){gfx.gremlinType1Gfx, gfx.gremlinType1Gfx+64});
      break;
    case 1:
      setGfxPtrs((u16*[]){gfx.gremlinType2Gfx, gfx.gremlinType2Gfx+64});
      break;
    default:
      setGfxPtrs((u16*[]){gfx.gremlinType3Gfx, gfx.gremlinType3Gfx+64});
      break;
  }
  show();
  iprintf("drawing gremlin\n");
  drawUpdate();
}

Gremlin::~Gremlin()
{
  iprintf("destroying gremlin %p\n",this);
}

void Gremlin::drawUpdate()
{
  switch (state) {
    case Paused:
      if((rand() & 7) == 7)
      {
        dx = mod32(rand(),3) - 1;
        dy = mod32(rand(),3) - 1;
        checkBounds();
        state = Moving;
      }
      break;
    case Moving:
      x += dx;
      y += dy;
      if((x & 15) == 0 && (y & 15) == 0)
      {
        if(associatedLevel->getTileAt(x >> 4, y >> 4)->isInteractive())
        {
          state = Screwing;
        }
        else
        {
          state = Paused;
        }
      }
      break;
    case Screwing:
      if(frameCounter == 0)
      {
        dx = mod32(rand(),3) - 1;
        if((rand() & 3) == 3)
        {
          dy = mod32(rand(),3) - 1;
          checkBounds();
          state = Moving;
        }
      }
      Tile* t = associatedLevel->getTileAt(x >> 4, y >> 4);
      ((Mirror*)t)->rotate(dx);
      t->drawUpdate();
      break;
  }
  if(++frameCounter > FRAMES_PER_SPRITE_FRAME)
  {
    frameCounter = 0;
    if(++curFrame >= numFrames)
    {
      curFrame = 0;
    }
  }
  setPosition(coord(x+XOFS,y+YOFS));
  update();
}

bool Gremlin::tapOn()
{
  //Try to 'evade' the player...
  if(state == Paused || state == Screwing)
  {
    dx = mod32(rand(),3) - 1;
    dy = mod32(rand(),3) - 1;
    checkBounds();
    state = Moving;
  }
  return (++taps >= 4);
}