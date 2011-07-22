/*
 *  gremlin.h
 *  deflektor-ds
 *
 *  Created by Hugh Cole-Baker on 5/1/09.
 *  The 'gremlin' creatures that roam about the game board and randomize the mirrors
 */

#ifndef deflektor_gremlin_h
#define deflektor_gremlin_h

#include <nds.h>
#include "beam.h"
#include "sprite.h"

enum GremlinState {
  Moving,
  Paused,
  Screwing
};

class Level;

class Gremlin : public Sprite {
  unsigned int taps, frameCounter;
  int x, y, dx, dy;
  GremlinState state;
  //Holds a pointer to the level so it can alter mirrors
  Level* associatedLevel;
  
  inline void checkBounds()
  {
    if(x == 0 && dx == -1) dx = 1;
    if(x >= 224 && dx == 1) dx = -1;
    if(y == 0 && dy == -1) dy = 1;
    if(y >= 128 && dy == 1) dy = -1;
  }
public:
  Gremlin(Level* lev);
  virtual ~Gremlin();
  void drawUpdate();
  bool tapOn();
  coord getPos();
};

#endif