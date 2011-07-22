/*
 *  beam.h
 *  deflektor-ds
 *
 *  Created by Hugh Cole-Baker on 14/12/2008.
 *  Manages drawing the laser beam on a bitmap background
 *  Also contains some enums to store information about how it interacts with the board
 */

#ifndef deflektor_beam_h
#define deflektor_beam_h

#include <utility>

using namespace std;

typedef pair<int, int> coord;

typedef enum {
  Pass,
  Block,
  Reflect,
  Teleport,
  Overload,
  Complete
} BeamAction;

typedef enum {
  N,
  NNE,
  NE,
  ENE,
  E,
  ESE,
  SE,
  SSE,
  S,
  SSW,
  SW,
  WSW,
  W,
  WNW,
  NW,
  NNW
} BeamDirection;

class BeamResult
{
public:
  BeamAction action;
  unsigned int xPos;
  unsigned int yPos;
  BeamDirection direction;
  unsigned int flag;
};

class Beam {
protected:
  static const int dx[];
  static const int dy[];
  const int bgId;
  unsigned int color;
public:
  void drawLine(int x1, int y1, int x2, int y2);
  void draw(int x1, int y1, int x2, int y2);
  void drawBox(int x1, int y1, int x2, int y2, unsigned short boxColor);
  Beam(int bg, unsigned int col);
  virtual ~Beam();
  void clear();
  static inline coord calculateNextPos(unsigned int x, unsigned int y, BeamDirection angle)
  {
    return coord(x+dx[angle],y+dy[angle]);
  }
  
  static BeamResult reflectVertical(BeamDirection initial);
  static BeamResult reflectHorizontal(BeamDirection initial);
};

#endif