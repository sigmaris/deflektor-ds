/*
 *  affinesprite.h
 *  deflektor-ds
 *
 *  A Sprite that can also have an affine transform applied to it.
 */

#ifndef deflektor_affinesprite_h
#define deflektor_affinesprite_h

#include "sprite.h"

class AffineSprite : public Sprite
{
private:
  static int freeBitmap;
protected:
  unsigned int affineIndex;
  int angle;
  int sx;
  int sy;
  const bool allocateAffine;
  bool doubleSize;
  virtual void update();
  virtual bool init();
public:
  AffineSprite(bool screen, int palIdx, SpriteSize sz, SpriteColorFormat fmt,
               int frames, bool allocAffine);
  AffineSprite(bool screen, coord pos, int prio, int palIdx,
               SpriteSize sz, SpriteColorFormat fmt,
               bool hide, bool hFlip, bool vFlip, bool mosaic, int frames,
               bool allocateGfx, bool allocAffine, bool doubleSz);
  virtual ~AffineSprite();
};

#endif