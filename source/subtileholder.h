/*
 *  subtileholder.h
 *  deflektor-ds
 *
 *  Created by Hugh Cole-Baker on 1/20/09.
 *  Holds a tile made of 4 smaller subtiles that can reflect the beam
 *
 */

#ifndef deflektor_subtileholder_h
#define deflektor_subtileholder_h

#include "tile.h"
#include "subtile.h"

class SubtileHolder : public Tile
{
  protected:
    /* Subtiles stored in this order:
     *  0 1
     *  2 3
     */
    Subtile* subtiles[4];
    bool destroyed;
  public:
    SubtileHolder(int bg, int x, int y, unsigned int palIdx, Subtile** tiles);
    virtual ~SubtileHolder(); 
    virtual void drawUpdate();
    virtual bool isInteractive();
    virtual BeamResult beamEnters(unsigned int x, unsigned int y, BeamDirection atAngle);
    virtual void destroy();
};

#endif