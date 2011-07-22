/*
 *  spinreflector.cpp
 *  deflektor-ds
 *
 *  Created by Hugh Cole-Baker on 18/12/2008.
 *
 */

#include <nds.h>
#include <stdio.h>

#include "spinreflector.h"
#include "laser.h"

SpinReflector::SpinReflector(int bg, int x, int y, unsigned int palIdx, unsigned int tile, BeamDirection initAngle, int ref) : SpinBlocker(bg, x, y, palIdx, tile+SPIN_BLOCKER_TILES, initAngle), reflectiveSides(ref)
{
  angle = (initAngle & 7);
  drawUpdate();
}

BeamResult SpinReflector::beamEnters(unsigned int x, unsigned int y, BeamDirection atAngle)
{
  BeamResult res;
  if((atAngle & 7) == angle)
  {
    res.action = Pass;
  }
  else
  {
    res.action = Reflect;
    unsigned int relX = x - xPos * 4;
    unsigned int relY = y - yPos * 4;
    
    if(relX == 0)
    {
      if(relY == 0)
      {
        if((reflectiveSides & UP) && (reflectiveSides & LEFT))
        {
          //top left corner
          switch(atAngle)
          {
            case ESE:
              res.direction = NNW;
              break;
            case SE:
              res.action = Overload;
              res.flag = 1;
              break;
            case SSE:
              res.direction = WNW;
              break;
            default:
              res.action = Pass;
          }
        }
        else if(reflectiveSides & UP)
        {
          res = Beam::reflectHorizontal(atAngle);
        }
        else if(reflectiveSides & LEFT)
        {
          res = Beam::reflectVertical(atAngle);
        }
        else
        {
          res.action = Block;
        }
      }
      else if(relY == 4)
      {
        if((reflectiveSides & DOWN) && (reflectiveSides & LEFT))
        {
          //bottom left corner
          switch(atAngle)
          {
            case ENE:
              res.direction = SSW;
              break;
            case NE:
              res.action = Overload;
              res.flag = 1;
              break;
            case NNE:
              res.direction = WSW;
              break;
            default:
              res.action = Pass;
          }
        }
        else if(reflectiveSides & DOWN)
        {
          res = Beam::reflectHorizontal(atAngle);
        }
        else if(reflectiveSides & LEFT)
        {
          res = Beam::reflectVertical(atAngle);
        }
        else
        {
          res.action =Block;
        }
      }
      else
      {
        res = Beam::reflectVertical(atAngle);
      }
    }
    else if(relX == 4)
    {
      if(relY == 0)
      {
        if((reflectiveSides & UP) && (reflectiveSides & RIGHT))
        {
          //top right corner
          switch(atAngle)
          {
            case WSW:
              res.direction = NNE;
              break;
            case SW:
              res.action = Overload;
              res.flag = 1;
              break;
            case SSW:
              res.direction = ENE;
              break;
            default:
              res.action = Pass;
          }
        }
        else if(reflectiveSides & UP)
        {
          res = Beam::reflectHorizontal(atAngle);
        }
        else if(reflectiveSides & RIGHT)
        {
          res = Beam::reflectVertical(atAngle);
        }
        else
        {
          res.action = Block;
        }
      }
      else if(relY == 4)
      {
        if((reflectiveSides & DOWN) && (reflectiveSides & RIGHT))
        {
          //bottom right corner
          switch(atAngle)
          {
            case WNW:
              res.direction = SSE;
              break;
            case NW:
              res.action = Overload;
              res.flag = 1;
              break;
            case NNW:
              res.direction = ESE;
              break;
            default:
              res.action = Pass;
          }
        }
        else if(reflectiveSides & DOWN)
        {
          res = Beam::reflectHorizontal(atAngle);
        }
        else if(reflectiveSides & RIGHT)
        {
          res = Beam::reflectVertical(atAngle);
        }
        else
        {
          res.action = Block;
        }
      }
      else
      {
        res = Beam::reflectVertical(atAngle);
      }
    }
    else
    {
      res = Beam::reflectHorizontal(atAngle);
    }
    res.xPos = x;
    res.yPos = y;
  }
  return res;
}
