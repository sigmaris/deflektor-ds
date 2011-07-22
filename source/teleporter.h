/*
 *  teleporter.h
 *  deflektor-ds
 *
 *  Created by Hugh Cole-Baker on 1/18/09.
 *  Teleports the beam to another location (must be paired with another teleporter!).
 *
 */

#ifndef deflektor_teleporter_h
#define deflektor_teleporter_h

#include "tile.h"

class Teleporter : public Tile {
protected:
  const unsigned int tileBase;
  //The other teleporter to link with.
  Teleporter* link;
  bool destroyed;
public:
  Teleporter(unsigned int bg, unsigned int x, unsigned int y, unsigned int palIdx, unsigned int tile);
  void setLink(Teleporter* toLink);
  virtual void drawUpdate();
  virtual BeamResult beamEnters(unsigned int x, unsigned int y, BeamDirection atAngle);
  virtual void destroy();
};

#endif