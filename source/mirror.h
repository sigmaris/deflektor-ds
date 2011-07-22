#ifndef deflektor_mirror_h
#define deflektor_mirror_h

#include "tile.h"

class Mirror : public Tile
{
protected:
  const unsigned int mirrorTileBase;
  const int mirrorBgHandle;
  unsigned int angle;
  bool needsUpdate;
public:
  Mirror(int bg, int mirrorBg, int x, int y, unsigned int palIdx, unsigned int tileBase);
  virtual ~Mirror();
  void rotate(int amount);
  void setAngle(int angleSet);
  virtual BeamResult beamEnters(unsigned int x, unsigned int y, BeamDirection atAngle);
  virtual void drawUpdate();
  virtual bool isInteractive();
};

#endif