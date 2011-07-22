/*
 *  spike.h
 *  deflektor-ds
 *
 *  Created by Hugh Cole-Baker on 17/12/2008.
 *  The 'spikes' sap your energy when the beam hits them
 *
 */

#ifndef deflektor_spike_h
#define deflektor_spike_h

#include "tile.h"

class Spike : public Tile
{
protected:
  const unsigned int tileBase;
  bool destroyed;
public:
  Spike(int bg, int x, int y, unsigned int palIdx, unsigned int tile);
  virtual void drawUpdate();
  virtual BeamResult beamEnters(unsigned int x, unsigned int y, BeamDirection atAngle);
  virtual void destroy();
};

#endif