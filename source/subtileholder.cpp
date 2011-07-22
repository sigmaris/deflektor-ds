/*
 *  subtileholder.cpp
 *  deflektor-ds
 *
 *  Created by Hugh Cole-Baker on 1/20/09.
 *
 */

#include "subtileholder.h"
#include "beam.h"
#include "laser.h"
#include <string.h>
#include <nds.h>
#include <stdio.h>

SubtileHolder::SubtileHolder(int bg, int x, int y, unsigned int palIdx, Subtile** tiles) : Tile(bg,x,y,palIdx), destroyed(false)
{
  memcpy(subtiles, tiles, sizeof(Subtile*)*4);
  if(subtiles[0])
  {
    subtiles[0]->setXPos(4*x);
    subtiles[0]->setYPos(4*y);
    subtiles[0]->setBgHandle(bg);
    subtiles[0]->setPalIdx(palIdx);
  }
  if(subtiles[1])
  {
    subtiles[1]->setXPos(4*x + 2);
    subtiles[1]->setYPos(4*y);
    subtiles[1]->setBgHandle(bg);
    subtiles[1]->setPalIdx(palIdx);
  }
  if(subtiles[2])
  {
    subtiles[2]->setXPos(4*x);
    subtiles[2]->setYPos(4*y + 2);
    subtiles[2]->setBgHandle(bg);
    subtiles[2]->setPalIdx(palIdx);
  }
  if(subtiles[3])
  {
    subtiles[3]->setXPos(4*x + 2);
    subtiles[3]->setYPos(4*y + 2);
    subtiles[3]->setBgHandle(bg);
    subtiles[3]->setPalIdx(palIdx);
  }
  drawUpdate();
}

SubtileHolder::~SubtileHolder()
{
  for (int n = 0; n < 4; ++n)
  {
    if(subtiles[n]) delete subtiles[n];
  }
}

void SubtileHolder::drawUpdate()
{
  for (int n = 0; n < 4; ++n) {
    if(subtiles[n]) subtiles[n]->drawUpdate();
  }
}

bool SubtileHolder::isInteractive()
{
  return false;
}

BeamResult SubtileHolder::beamEnters(unsigned int x, unsigned int y, BeamDirection atAngle)
{
  BeamResult res;
  res.action = Pass;
  
  if(destroyed) return res;
  
  unsigned int relX = x - xPos * 4;
  unsigned int relY = y - yPos * 4;
  unsigned int reflectiveSides = 0;
  
  //normal condition
  unsigned int subX = relX / 2;
  unsigned int subY = relY / 2;
  if(relX % 2 == 0 && atAngle > S) subX = relX / 2 - 1;
  if(relY % 2 == 0 && (atAngle > W || atAngle < E)) subY = relY / 2 - 1;
  if(subX > 1 || subY > 1) iprintf("subtile out of range at (%d,%d)!",x,y);
  unsigned int idx = subX + subY*2;
  if(subtiles[idx]) res = subtiles[idx]->beamEnters(x, y, atAngle);
  
  //special check for border between 2 subtiles
  if(relX == 2)
  {
    if(relY > 2)
    {
      if(subtiles[2] && subtiles[3] && (atAngle == N || atAngle == S))
      {
        reflectiveSides = (subtiles[2]->getReflectiveSides()) | (subtiles[3]->getReflectiveSides());
        res.action = Block;
      }
    }
    else
    {
      if(subtiles[0] && subtiles[1] && (atAngle == N || atAngle == S))
      {
        reflectiveSides = (subtiles[0]->getReflectiveSides()) | (subtiles[1]->getReflectiveSides());
        res.action = Block;
      }
    }
    if(((atAngle <= E || atAngle >= W) && (reflectiveSides & DOWN)) || (atAngle >= E && atAngle <= W && (reflectiveSides & UP)))
    { 
      res = Beam::reflectHorizontal(atAngle);
      res.xPos = x;
      res.yPos = y;
    }
    if(relY == 2)
    {
      if(subtiles[0] && subtiles[3])
      {
        if((subtiles[0]->getReflectiveSides() & RIGHT) && 
           (subtiles[3]->getReflectiveSides() & UP) &&
           atAngle <= W && atAngle >= S)
        {
          res.xPos = x;
          res.yPos = y;
          switch (atAngle) {
            case S:
              res.action = Overload;
              res.flag = 1;
              break;
            case SSW:
              res.action = Reflect;
              res.direction = ENE;
              break;
            case SW:
              if(res.action == Block)
              {
                res.action = Overload;
                res.flag = 1;
              }
              break;
            case WSW:
              res.action = Reflect;
              res.direction = NNE;
              break;
            case W:
              res.action = Overload;
              res.flag = 1;
              break;
            default:
              break;
          }
        }
        if((subtiles[0]->getReflectiveSides() & DOWN) && 
           (subtiles[3]->getReflectiveSides() & LEFT) &&
           atAngle <= E)
        {
          res.xPos = x;
          res.yPos = y;
          switch (atAngle) {
            case N:
              res.action = Overload;
              res.flag = 1;
              break;
            case NNE:
              res.action = Reflect;
              res.direction = WSW;
              break;
            case NE:
              if(res.action == Block)
              {
                res.action = Overload;
                res.flag = 1;
              }
              break;
            case ENE:
              res.action = Reflect;
              res.direction = SSW;
              break;
            case E:
              res.action = Overload;
              res.flag = 1;
              break;
            default:
              break;
          }
        }
      }
      if(subtiles[1] && subtiles[2])
      {
        if((subtiles[1]->getReflectiveSides() & LEFT) &&
           (subtiles[2]->getReflectiveSides() & UP) &&
           atAngle >= E && atAngle <= S)
        {
          res.xPos = x;
          res.yPos = y;
          switch (atAngle) {
            case E:
              res.action = Overload;
              res.flag = 1;
              break;
            case ESE:
              res.action = Reflect;
              res.direction = NNW;
              break;
            case SE:
              if(res.action == Block)
              {
                res.action = Overload;
                res.flag = 1;
              }
              break;
            case SSE:
              res.action = Reflect;
              res.direction = WNW;
              break;
            case S:
              res.action = Overload;
              res.flag = 1;
              break;
            default:
              break;
          }
        }
        if((subtiles[1]->getReflectiveSides() & DOWN) &&
           (subtiles[2]->getReflectiveSides() & RIGHT) &&
           (atAngle >= W || atAngle == N))
        {
          res.xPos = x;
          res.yPos = y;
          switch (atAngle) {
            case W:
              res.action = Overload;
              res.flag = 1;
              break;
            case WNW:
              res.action = Reflect;
              res.direction = SSE;
              break;
            case NW:
              if(res.action == Block)
              {
                res.action = Overload;
                res.flag = 1;
              }
              break;
            case NNW:
              res.action = Reflect;
              res.direction = ESE;
              break;
            case N:
              res.action = Overload;
              res.flag = 1;
              break;
            default:
              break;
          }
        }
      }
    }
    if(((!subtiles[0] && !subtiles[2]) || (!subtiles[1] && !subtiles[3])) && (atAngle == N || atAngle == S))
    {
      res.action = Pass;
    }
    if(((!subtiles[0] && !subtiles[1]) || (!subtiles[2] && !subtiles[3])) && (atAngle == W || atAngle == E))
    {
      res.action = Pass;
    }
  }
  else if(relY == 2)
  {
    if(relX < 2)
    {
      if(subtiles[0] && subtiles[2] && (atAngle == E || atAngle == W))
      {
        reflectiveSides = (subtiles[0]->getReflectiveSides()) | (subtiles[2]->getReflectiveSides());
        res.action = Block;
        //iprintf("blocking left(%d,%d)\n",xPos,yPos);
      }
    }
    else
    {
      if(subtiles[1] && subtiles[3] && (atAngle == E || atAngle == W))
      {
        reflectiveSides = (subtiles[1]->getReflectiveSides()) | (subtiles[3]->getReflectiveSides());
        res.action = Block;
        //iprintf("blocking right(%d,%d)\n",xPos,yPos);
      }
    }
    if((atAngle >= S && (reflectiveSides & RIGHT)) || (atAngle <= S && (reflectiveSides & LEFT)))
    {
      //iprintf("reflecting instead");
      res = Beam::reflectVertical(atAngle);
      res.xPos = x;
      res.yPos = y;
    }
    if(((!subtiles[0] && !subtiles[1]) || (!subtiles[2] && !subtiles[3])) && (atAngle == W || atAngle == E))
    {
      res.action = Pass;
    }
  }
  return res;
}

void SubtileHolder::destroy()
{
  destroyed = true;
  for (int n = 0; n < 4; ++n)
  {
    if(subtiles[n]) subtiles[n]->destroy();
  }
}