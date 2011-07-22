/*
 *  receiver.h
 *  deflektor-ds
 *
 *  Created by Hugh Cole-Baker on 14/12/2008.
 *  This is the tile that the player has to shoot to finish the level.
 *
 */

#ifndef deflektor_receiver_h
#define deflektor_receiver_h

#include "tile.h"
#include "laser.h"

class Receiver : public Tile
{
protected:
  unsigned int receiverTileBase;
  LaserDirection dir;
public:
  Receiver(int bg, int x, int y, unsigned int palIdx, unsigned int tileBase, LaserDirection direction);
  virtual void drawUpdate();
  virtual BeamResult beamEnters(unsigned int x, unsigned int y, BeamDirection atAngle);
};

#endif