/*
 *  sprite.h
 *  deflektor-ds
 *
 *  A class managing a hardware sprite
 */

#ifndef deflektor_sprite_h
#define deflektor_sprite_h

#include "beam.h"
#include <nds.h>

class Sprite
{
private:
  static int freeBitmap[4];
protected:
  const bool mainScreen;
  int spriteIndex;
  coord position;
  int priority;
  int palIndex;
  SpriteSize size;
  SpriteColorFormat colFmt;
  u16** gfxPtr;
  bool hidden;
  bool flipH;
  bool flipV;
  bool applyMosaic;
  const int numFrames;
  int curFrame;
  const bool managedGfx;
  virtual void update();
  virtual bool init();
public:
  Sprite(bool screen, int palIdx, SpriteSize sz, SpriteColorFormat fmt, int frames);
  Sprite(bool screen, coord pos, int prio, int palIdx,
         SpriteSize sz, SpriteColorFormat fmt,
         bool hide, bool hFlip, bool vFlip, bool mosaic, int frames, bool allocateGfx);
  virtual ~Sprite();
  void setPosition(coord pos);
  void setXPos(int xPos);
  void setYPos(int yPos);
  void setGfxPtrs(u16* gfxPtrs[]);
  u16** getGfxPtrs();
  coord& getPosition();
  void setAlphaBlend(bool alphaBlend);
  void show();
  void hide();
};

#endif