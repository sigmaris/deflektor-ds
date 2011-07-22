#ifndef deflektor_tile_h
#define deflektor_tile_h

#define TILES_X 15
#define TILES_Y 9
#define ANIMATION_DELAY 3

#include "beam.h"

/*
 * Class represents a blank tile on the 15x9 game map
 * is base class 
 */
class Tile
{
protected:
  const int bgHandle;
  unsigned int xPos;
  unsigned int yPos;
  unsigned int paletteIndex;
public:
  Tile(int bg, int x, int y, unsigned int palIdx);
  virtual ~Tile();
  //Called once per frame
  virtual void drawUpdate();
  //Can the tile be interacted with? (mirrors only)
  virtual bool isInteractive();
  //What happens when the beam enters this tile at a point and angle?
  virtual BeamResult beamEnters(unsigned int x, unsigned int y, BeamDirection atAngle);
  static inline unsigned int pos2idx(unsigned int x, unsigned int y)
  {
    return (32*y) + x;
  }
  //Helper to find if the beam hits the centre of this tile
  inline bool hitsCenter(unsigned int x, unsigned int y, BeamDirection atAngle)
  {
    unsigned int relX = x - 4*xPos;
    unsigned int relY = y - 4*yPos;
    return (relX == 2 && relY == 2);
  }
  //Called to destroy blocks that are blocking the exit, when all targets have been hit
  virtual void destroy();
  virtual coord getPosition();
};

#endif