#include <nds.h>
#include <stdio.h>
#include "beam.h"

void Beam::clear()
{
  //clear 145 lines - the part covering the game area
  //dmaFillWords(0, (void*)bgGetGfxPtr(bgId), 2*SCREEN_WIDTH*145);
  //or - The following is equivalent to dmaFillWordsAsync
  DMA_FILL(3) = (vuint32)0;
  DMA_SRC(3) = (uint32)&DMA_FILL(3);
  DMA_DEST(3) = (uint32)bgGetGfxPtr(bgId);
  DMA_CR(3) = DMA_SRC_FIX | DMA_COPY_WORDS | ((SCREEN_WIDTH*145)>>1);
}

//Draws the beam in tile co-ordinates (1/4 of screen co-ordinates)
void Beam::draw(int x1, int y1, int x2, int y2)
{
  drawLine(x1*4, y1*4, x2*4, y2*4);
}

//Draws a rectangle around a certain area
void Beam::drawBox(int x1, int y1, int x2, int y2, u16 boxColor)
{
  u16* bitmapPtr = (u16*)bgGetGfxPtr(bgId);
  if(x2 < x1)
  {
    x2 ^= x1;
    x1 ^= x2;
    x2 ^= x1;
  }
  if(y2 < y1)
  {
    y2 ^= y1;
    y1 ^= y2;
    y2 ^= y1;
  }
  dmaFillHalfWords(boxColor, (void*)(bitmapPtr + (y1 * SCREEN_WIDTH + x1)), (x2 - x1) << 1);
  dmaFillHalfWords(boxColor, (void*)(bitmapPtr + (y2 * SCREEN_WIDTH + x1)), (x2 - x1) << 1);
  for(int pos = y1; pos <= y2; ++ pos)
  {
    bitmapPtr[pos*SCREEN_WIDTH + x1] = boxColor;
    bitmapPtr[pos*SCREEN_WIDTH + x2] = boxColor;
  }
}

/* 
 * Implementation of Bresenham Line algorithm from
 * http://www.dev-scene.com/NDS/Tutorials_Day_3
 *
 */
void Beam::drawLine(int x1, int y1, int x2, int y2)
{
  volatile u16* bitmapPtr = (volatile u16*)bgGetGfxPtr(bgId);
  int yStep = SCREEN_WIDTH;
  int xStep = 1;
  int xDiff = x2 - x1;
  int yDiff = y2 - y1;
  
  int errorTerm = 0;
  int offset = y1 * SCREEN_WIDTH + x1;
  int i;
  
  //need to adjust if y1 > y2
  if (yDiff < 0)
  {
    yDiff = -yDiff;   //absolute value
    yStep = -yStep;   //step up instead of down
  }
  
  //same for x
  if (xDiff < 0)
  {
    xDiff = -xDiff;
    xStep = -xStep;
  }
  
  //case for changes more in X than in Y
  if (xDiff > yDiff)
  {                            
    for (i = 0; i < xDiff + 1; i++)
    {
      bitmapPtr[offset] = (u16)color;
      bitmapPtr[offset+yStep] = (u16)color;
      
      offset += xStep;
      
      errorTerm += yDiff;
      
      if (errorTerm > xDiff)
      {
        errorTerm -= xDiff;
        offset += yStep;
      }
    }
  }//end if xdiff > ydiff
  //case for changes more in Y than in X
  else
  {
    for (i = 0; i < yDiff + 1; i++)
    {
      bitmapPtr[offset] = (u16)color;
      bitmapPtr[offset+xStep] = (u16)color;
      
      offset += yStep;
      
      errorTerm += xDiff;
      
      if (errorTerm > yDiff)
      {
        errorTerm -= yDiff;
        offset += xStep;
      }
    }
  }
}

const int Beam::dx[] = {0,1,2,2,2,2,2,1,0,-1,-2,-2,-2,-2,-2,-1};
const int Beam::dy[] = {-2,-2,-2,-1,0,1,2,2,2,2,2,1,0,-1,-2,-2};

BeamResult Beam::reflectVertical(BeamDirection initial)
{
  BeamResult res;
  res.action = Reflect;
  switch (initial)
  {
    case N:
      res.direction = N;
      break;
    case NNE:
      res.direction = NNW;
      break;
    case NE:
      res.direction = NW;
      break;
    case ENE:
      res.direction = WNW;
      break;
    case E:
      res.direction = W;
      res.action = Overload;
      res.flag = 1;
      break;
    case ESE:
      res.direction = WSW;
      break;
    case SE:
      res.direction = SW;
      break;
    case SSE:
      res.direction = SSW;
      break;
    case S:
      res.direction = S;
      break;
    case SSW:
      res.direction = SSE;
      break;
    case SW:
      res.direction = SE;
      break;
    case WSW:
      res.direction = ESE;
      break;
    case W:
      res.direction = E;
      res.action = Overload;
      res.flag = 1;
      break;
    case WNW:
      res.direction = ENE;
      break;
    case NW:
      res.direction = NE;
      break;
    case NNW:
      res.direction = NNE;
      break;
    default:
      iprintf("Invalid value for reflectVertical: %d",initial);
      break;
  }
  return res;
}

BeamResult Beam::reflectHorizontal(BeamDirection initial)
{
  BeamResult res;
  res.action = Reflect;
  switch (initial)
  {
    case N:
      res.direction = S;
      res.action = Overload;
      res.flag = 1;
      break;
    case NNE:
      res.direction = SSE;
      break;
    case NE:
      res.direction = SE;
      break;
    case ENE:
      res.direction = ESE;
      break;
    case E:
      res.direction = E;
      break;
    case ESE:
      res.direction = ENE;
      break;
    case SE:
      res.direction = NE;
      break;
    case SSE:
      res.direction = NNE;
      break;
    case S:
      res.direction = N;
      res.action = Overload;
      res.flag = 1;
      break;
    case SSW:
      res.direction = NNW;
      break;
    case SW:
      res.direction = NW;
      break;
    case WSW:
      res.direction = WNW;
      break;
    case W:
      res.direction = W;
      break;
    case WNW:
      res.direction = WSW;
      break;
    case NW:
      res.direction = SW;
      break;
    case NNW:
      res.direction = SSW;
      break;
    default:
      iprintf("Invalid value for reflectHorizontal: %d",initial);
      break;
  }
  return res;
}

Beam::Beam(int bg, unsigned int col) : bgId(bg), color(col)
{
}

Beam::~Beam()
{
  clear();
}
