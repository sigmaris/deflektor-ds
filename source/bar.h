/*
 *  bar.h
 *  deflektor-ds
 *
 *  Created by Hugh Cole-Baker on 5/1/09.
 *  Manages the life and time bars at the bottom of the screen
 *
 */

#ifndef deflektor_bar_h
#define deflektor_bar_h

#include "affinesprite.h"

class Bar : public AffineSprite
{
  unsigned int value;
  int initX;
public:
  Bar(coord pos, u16* gfx, int palIdx);
  void setValue(unsigned int val);
};

#endif