/*
 *  subtile.cpp
 *  deflektor-ds
 *
 *  Created by Hugh Cole-Baker on 1/19/09.
 *
 */

#include "subtile.h"
#include "laser.h"
#include <stdio.h>
#include <nds.h>

Subtile::Subtile(unsigned int tile, unsigned int reflect, bool isGoal) : 
tileBase(reflect ? tile : tile + 1),
reflectiveSides(reflect),
goal(isGoal),
destroyed(false)
{
}

unsigned int Subtile::getReflectiveSides()
{
  return reflectiveSides;
}

void Subtile::setPalIdx(unsigned int pal)
{
  palIdx = pal;
}

void Subtile::setBgHandle(int bg)
{
  bgHandle = bg;
}

void Subtile::setXPos(unsigned int x)
{
  xPos = x;
}

void Subtile::setYPos(unsigned int y)
{
  yPos = y;
}

//yes, this function is kinda a mess :(
BeamResult Subtile::beamEnters(unsigned int x, unsigned int y, BeamDirection angle)
{
  unsigned int relX = x - xPos;
  unsigned int relY = y - yPos;
  BeamResult res;
  if(destroyed)
  {
    res.action = Pass;
  }
  else if(relY == 0)
  {
    //top left corner
    if(relX == 0 && ((reflectiveSides & UP && reflectiveSides & LEFT) || reflectiveSides & TOPLEFT))
    {
      switch (angle) {
        case E:
          res.action = (reflectiveSides & TOPLEFT ? Overload : Pass);
          res.flag = 1;
          break;
        case ESE:
          res.action = Reflect;
          res.direction = NNW;
          break;
        case SE:
          res.action = Overload;
          res.flag = 1;
          res.direction = NW;
          break;
        case SSE:
          res.action = Reflect;
          res.direction = WNW;
          break;
        case S:
          res.action = (reflectiveSides & TOPLEFT ? Overload : Pass);
          res.flag = 1;
          break;
        default:
          break;
      }
    }
    else if(relX == 2 && ((reflectiveSides & UP && reflectiveSides & RIGHT) || reflectiveSides & TOPRIGHT))
    {
      //top right corner
      switch (angle) {
        case S:
          res.action = (reflectiveSides & TOPRIGHT ? Overload : Pass);
          res.flag = 1;
          break;
        case SSW:
          res.action = Reflect;
          res.direction = ENE;
          break;
        case SW:
          res.action = Overload;
          res.flag = 1;
          res.direction = NE;
          break;
        case WSW:
          res.action = Reflect;
          res.direction = NNE;
          break;
        case W:
          res.action = (reflectiveSides & TOPRIGHT ? Overload : Pass);
          res.flag = 1;
        default:
          break;
      }
    }    
    else if(relX == 0 && reflectiveSides & LEFT)
    {
      //top left
      res = Beam::reflectVertical(angle);
    }
    else if(relX == 2 && reflectiveSides & RIGHT)
    {
      //top right
      res = Beam::reflectVertical(angle);
    }
    else if(reflectiveSides & UP)
    {
      //anywhere on top
      res = Beam::reflectHorizontal(angle);
    }
    else
    {
      res.action = Block;
    }
  }
  else if(relY == 2)
  {
    if(relX == 0 && ((reflectiveSides & DOWN && reflectiveSides & LEFT) || reflectiveSides & BOTTOMLEFT))
    {
      //bottom left corner
      switch (angle) {
        case E:
          res.action = (reflectiveSides & BOTTOMLEFT ? Overload : Pass);
          res.flag = 1;
          break;
        case ENE:
          res.action = Reflect;
          res.direction = SSW;
          break;
        case NE:
          res.action = Overload;
          res.flag = 1;
          res.direction = SW;
          break;
        case NNE:
          res.action = Reflect;
          res.direction = WSW;
          break;
        case N:
          res.action = (reflectiveSides & BOTTOMLEFT ? Overload : Pass);
          res.flag = 1;
          break;
        default:
          break;
      }
    }
    else if(relX == 2 && ((reflectiveSides & DOWN && reflectiveSides & RIGHT) || reflectiveSides & BOTTOMRIGHT))
    {
      //bottom right corner
      switch (angle) {
        case N:
          res.action = (reflectiveSides & BOTTOMRIGHT ? Overload : Pass);
          res.flag = 1;
          break;
        case NNW:
          res.action = Reflect;
          res.direction = ESE;
          break;
        case NW:
          res.action = Overload;
          res.flag = 1;
          res.direction = SE;
          break;
        case WNW:
          res.action = Reflect;
          res.direction = SSE;
          break;
        case W:
          res.action = (reflectiveSides & BOTTOMRIGHT ? Overload : Pass);
          res.flag = 1;
        default:
          break;
      }
    }
    else if(relX == 0 && reflectiveSides & LEFT)
    {
      res = Beam::reflectVertical(angle);
    }
    else if(relX == 2 && reflectiveSides & RIGHT)
    {
      res = Beam::reflectVertical(angle);
    }
    else if(reflectiveSides & DOWN)
    {
      res = Beam::reflectHorizontal(angle);
    }
    else
    {
      res.action = Block;
    }
  }
  else if(relX == 0)
  {
    if(reflectiveSides & LEFT) res = Beam::reflectVertical(angle);
    else res.action = Block;
  }
  else if(relX == 2)
  {
    if(reflectiveSides & RIGHT) res = Beam::reflectVertical(angle);
    else res.action = Block;
  }
  else
  {
    iprintf("beam in subtile(%d,%d) at (%d,%d)!\n", xPos, yPos, x,y);
    res.action = Block;
  }
  res.xPos = x;
  res.yPos = y;
  return res;
}

bool Subtile::isGoal()
{
  return goal;
}

void Subtile::destroy()
{
  destroyed = true;
  drawUpdate();
}

void Subtile::drawUpdate()
{
  volatile u16* mapPtr = (volatile u16*)bgGetMapPtr(bgHandle);
  mapPtr[16 * yPos + xPos / 2] = (destroyed ? 0 : ((palIdx << 12) | tileBase));
}