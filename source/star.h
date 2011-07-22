/*
 *  star.h
 *  deflektor-ds
 *
 *  Created by Hugh Cole-Baker on 5/16/09.
 *  Manages the star sprites on the title screen
 *
 */

#ifndef deflektor_star_h
#define deflektor_star_h

#include "sprite.h"

class Star : public Sprite {
  int dx, dy;
  //fx and fy are the x and y position *4 (for smooth animation)
  unsigned int fx, fy;
public:
  Star(int palIdx);
  void drawUpdate();
};

#endif