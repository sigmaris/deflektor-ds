%option noyywrap
%{

/* 
 * Parser (really just a lexer) for the level file format.
 * Not very complete in terms of checking the level is
 * valid, etc.
 */
#include <setjmp.h>

#include "settings.h"
#include "beam.h"
#include "tile.h"
#include "subtile.h"
#include "subtileholder.h"
#include "mirror.h"
#include "spinmirror.h"
#include "spinblocker.h"
#include "spinreflector.h"
#include "angleblocker.h"
#include "anglereflector.h"
#include "teleporter.h"
#include "laser.h"
#include "receiver.h"
#include "beam.h"
#include "spike.h"
#include "random.h"
#include "target.h"
#include "level.h"

#define MAX_TELEPORTS 16

#define SUBTILE_OFS 32
#define RANDOM_OFS 0
#define TP_OFS 28
#define TARGET_OFS 8
#define SPIKE_OFS 4

const int BASESTATE = 0;
const int GLOBAL = 1;
const int PALETTE = 2;
const int SUBTILES = 4;
const int TELEPORTER = 8;
const int ANGLEBLOCK = 16;
const int LASER = 32;
const int RECEIVER = 64;
const int REFLECT = 128;
const int BLOCK = 256;
const int BLANK = 512;
const int YPOS = 1024;
const int TILESPEC = 2048;
const int MIRROR = 4096;
const int TARGET = 8192;
const int SPINBLOCKER = 16384;
const int SPINREFLECTOR = 32768;
const int SPINMIRROR = 65536;
const int SPIKE = 131072;
const int ANGLEBLOCKER = 262144;
const int ANGLEREFLECTOR = 524288;
const int RANDOM = 1048576;
const int GOAL_SUBTILES = 2097152;
const int GOAL_SPIKE = 4194304;

struct SubtileConstruct {
  int type;
  int reflectiveSides;
};

struct Construct {
  int xPos;
  int yPos;
  int direction;
  int angle;
  int type;
  SubtileConstruct subtiles[4];
  int teleportSet;
  bool isGoal;
};

static int num_lines = 0;
static int subtileIdx = -1;
static int paletteIndex = -1;
static bool gremlins = false;
static int state;
static int paren;
#ifdef ARM9
static Tile* (*levelmap)[TILES_X][TILES_Y];
static Level* thisLevel;
static pair<Teleporter*, Teleporter*> teleporters[MAX_TELEPORTS];
#endif
static int tilesBg, mirrorsBg;
static unsigned int spinnersTileBase, objsTileBase, mirrorsTileBase, shipsTileBase;
static jmp_buf exception_env;

static Construct toConstruct = { -1, -1, -1, -1, -1, { {-1, 0}, {-1, 0}, {-1, 0}, {-1, 0} }, -1, false};

void initConstruct()
{
  state = BASESTATE;
  paren = 0;
  subtileIdx = -1;
  toConstruct = ((Construct){ -1, -1, -1, -1, -1, { {-1, 0}, {-1, 0}, {-1, 0}, {-1, 0} }, -1, false});
}

void parse_error(char* errorString)
{
#ifdef ARM9
  iprintf("parse_error at line %d, state %d, paren %d:",num_lines, state, paren);
  iprintf(errorString);
  iprintf("\n");
#else
  printf("parse_error at line %d, state %d, paren %d:",num_lines, state, paren);
  printf(errorString);
  printf("\n");
#endif
  longjmp(exception_env, 1);
}

Corner txtToCorner(char* text)
{
  if(strncmp(text, "tl", 2) == 0) return TOPLEFT;
  else if(strncmp(text, "tr", 2) == 0) return TOPRIGHT;
  else if(strncmp(text, "bl", 2) == 0) return BOTTOMLEFT;
  else if(strncmp(text, "br", 2) == 0) return BOTTOMRIGHT;
  else return (Corner) 0;
}

LaserDirection txtToDirection(char* text)
{
  if(strncmp(text, "up", 2) == 0) return UP;
  else if(strncmp(text, "left", 4) == 0) return LEFT;
  else if(strncmp(text, "down", 4) == 0) return DOWN;
  else if(strncmp(text, "right", 5) == 0) return RIGHT;
  else return (LaserDirection) 0;
}

BeamDirection txtToAngle(char* text)
{
  if(strncmp(text, "nne", 3) == 0) return NNE;
  if(strncmp(text, "sse", 3) == 0) return SSE;
  if(strncmp(text, "nnw", 3) == 0) return NNW;
  if(strncmp(text, "ssw", 3) == 0) return SSW;
  if(strncmp(text, "ene", 3) == 0) return ENE;
  if(strncmp(text, "ese", 3) == 0) return ESE;
  if(strncmp(text, "wnw", 3) == 0) return WNW;
  if(strncmp(text, "wsw", 3) == 0) return WSW;
  if(strncmp(text, "ne", 2) == 0) return NE;
  if(strncmp(text, "se", 2) == 0) return SE;
  if(strncmp(text, "nw", 2) == 0) return NW;
  if(strncmp(text, "sw", 2) == 0) return SW;
  if(strncmp(text, "n", 1) == 0) return N;
  if(strncmp(text, "e", 1) == 0) return E;
  if(strncmp(text, "s", 1) == 0) return S;
  if(strncmp(text, "w", 1) == 0) return W;
  return (BeamDirection) 0;
}

void constructSubtiles(const Construct& params)
{
#ifdef ARM9
  Subtile* tiles[4];
  for(int n = 0; n < 4; ++n)
  {
    if(params.subtiles[n].type == BLOCK)
    {
      if(params.subtiles[n].reflectiveSides != 0)
      {
        parse_error("block subtile with reflective sides!");
      }
      else
      {
        tiles[n] = new Subtile(objsTileBase+SUBTILE_OFS, 0, false);
      }
    }
    else if(params.subtiles[n].type == REFLECT)
    {
      if(params.subtiles[n].reflectiveSides == 0)
      {
        tiles[n] = new Subtile(objsTileBase+SUBTILE_OFS, UP | DOWN | LEFT | RIGHT, false);
      }
      else
      {
        tiles[n] = new Subtile(objsTileBase+SUBTILE_OFS, params.subtiles[n].reflectiveSides, false);
      }
    }
    else if(params.subtiles[n].type == BLANK)
    {
      tiles[n] = NULL;
    }
    else
    {
      parse_error("unrecognised subtile type");
    }
  }
  (*levelmap)[params.xPos][params.yPos] = new SubtileHolder(tilesBg, params.xPos, params.yPos, paletteIndex, tiles);
  if(params.isGoal)
  {
    thisLevel->addGoalTile((*levelmap)[params.xPos][params.yPos]);
  }
#else
  printf("constructing SubtitleHolder at (%d,%d)\n",params.xPos,params.yPos);
#endif
}

void constructAngleblock(const Construct& params)
{
  if(params.angle < N || params.angle > NNW)
  {
    parse_error("tile angle out of range");
  }
  switch(params.type)
  {
    case ANGLEBLOCKER:
#ifdef ARM9
      (*levelmap)[params.xPos][params.yPos] = new AngleBlocker(tilesBg, params.xPos, params.yPos, paletteIndex, spinnersTileBase, (BeamDirection)params.angle);
#else
  printf("constructing AngleBlocker at (%d,%d)\n",params.xPos,params.yPos);
#endif
      break; 
    case ANGLEREFLECTOR:
#ifdef ARM9
      (*levelmap)[params.xPos][params.yPos] = new AngleReflector(tilesBg, params.xPos, params.yPos, paletteIndex, spinnersTileBase, (BeamDirection)params.angle, params.direction);
#else
  printf("constructing AngleReflector at (%d,%d)\n",params.xPos,params.yPos);
#endif
      break;
    case SPINBLOCKER:
#ifdef ARM9
      (*levelmap)[params.xPos][params.yPos] = new SpinBlocker(tilesBg, params.xPos, params.yPos, paletteIndex, spinnersTileBase, (BeamDirection)params.angle);
      thisLevel->addAnimatedTile((*levelmap)[params.xPos][params.yPos]);
#else
      printf("constructing SpinBlocker at (%d,%d)\n",params.xPos,params.yPos);
#endif
      break;
    case SPINREFLECTOR:
#ifdef ARM9
      (*levelmap)[params.xPos][params.yPos] = new SpinReflector(tilesBg, params.xPos, params.yPos, paletteIndex, spinnersTileBase, (BeamDirection)params.angle, params.direction);
      thisLevel->addAnimatedTile((*levelmap)[params.xPos][params.yPos]);
#else
      printf("constructing SpinReflector at (%d,%d)\n",params.xPos,params.yPos);
#endif
      break;
    case SPINMIRROR:
#ifdef ARM9
      (*levelmap)[params.xPos][params.yPos] = new SpinMirror(tilesBg, mirrorsBg, params.xPos, params.yPos, paletteIndex, mirrorsTileBase, (BeamDirection)params.angle);
      thisLevel->addAnimatedTile((*levelmap)[params.xPos][params.yPos]);
#else
      printf("constructing SpinMirror at (%d,%d)\n",params.xPos,params.yPos);
#endif
      break;
    default:
      parse_error("unknown type to construct with angle");
  }
}

void constructShip(const Construct& params)
{
  if(params.direction != UP &&
      params.direction != DOWN &&
      params.direction != LEFT &&
      params.direction != RIGHT)
  {
    parse_error("invalid direction for receiver");
  }
  switch(params.type)
  {
    case LASER:
#ifdef ARM9
      (*levelmap)[params.xPos][params.yPos] = new Laser(tilesBg, params.xPos, params.yPos, paletteIndex, shipsTileBase, (LaserDirection)params.direction);
      thisLevel->setSource((Laser*)(*levelmap)[params.xPos][params.yPos]);
#else
  printf("constructing Laser at (%d,%d)\n",params.xPos,params.yPos);
#endif
      break;
    case RECEIVER:
#ifdef ARM9
      (*levelmap)[params.xPos][params.yPos] = new Receiver(tilesBg, params.xPos, params.yPos, paletteIndex, shipsTileBase, (LaserDirection)params.direction);
      thisLevel->setReceiver((Receiver*)(*levelmap)[params.xPos][params.yPos]);
#else
  printf("constructing Receiver at (%d,%d)\n",params.xPos,params.yPos);
#endif
      break;
    default:
      parse_error("unknown type to construct with direction");
  }
}

void constructTeleporter(const Construct& params)
{
#ifdef ARM9
  if(params.teleportSet < 0 || params.teleportSet >= MAX_TELEPORTS)
  {
    parse_error("invalid number for teleports");
  }
  if(teleporters[params.teleportSet].first == NULL)
  {
    teleporters[params.teleportSet].first = new Teleporter(tilesBg, params.xPos, params.yPos, paletteIndex, objsTileBase+TP_OFS);
    (*levelmap)[params.xPos][params.yPos] = teleporters[params.teleportSet].first;
    if(params.isGoal)
      thisLevel->addGoalTile(teleporters[params.teleportSet].first);
  }
  else if(teleporters[params.teleportSet].second == NULL)
  {
    teleporters[params.teleportSet].second = new Teleporter(tilesBg, params.xPos, params.yPos, paletteIndex, objsTileBase+TP_OFS);
    teleporters[params.teleportSet].second->setLink(teleporters[params.teleportSet].first);
    teleporters[params.teleportSet].first->setLink(teleporters[params.teleportSet].second);
    (*levelmap)[params.xPos][params.yPos] = teleporters[params.teleportSet].second;
    if(params.isGoal)
      thisLevel->addGoalTile(teleporters[params.teleportSet].second);
  }
  else
  {
    parse_error("Already 2 teleports in this set");
  }
#else
  printf("constructing and linking Teleporter(%d) at (%d,%d)\n",params.teleportSet,params.xPos,params.yPos);
#endif
}

void construct(const Construct& params)
{
  if(params.xPos < 0 || params.xPos >= TILES_X || params.yPos < 0 || params.yPos >= TILES_Y)
  {
    parse_error("tile position out of range");
  }
  switch(params.type)
  {
    case SUBTILES:
      constructSubtiles(params);
      break;
    case TELEPORTER:
      constructTeleporter(params);
      break;
    case LASER:
    case RECEIVER:
      constructShip(params);
      break;
    case RANDOM:
#ifdef ARM9
      (*levelmap)[params.xPos][params.yPos] = new Random(tilesBg, params.xPos, params.yPos, paletteIndex, objsTileBase+RANDOM_OFS);
#else
      printf("constructing Random at (%d,%d)\n",params.xPos,params.yPos);
#endif
      break;
    case MIRROR:
#ifdef ARM9
      (*levelmap)[params.xPos][params.yPos] = new Mirror(tilesBg, mirrorsBg, params.xPos, params.yPos, paletteIndex, mirrorsTileBase);
#else
      printf("constructing Mirror at (%d,%d)\n",params.xPos,params.yPos);
#endif
      break;
    case TARGET:
#ifdef ARM9
      (*levelmap)[params.xPos][params.yPos] = new Target(tilesBg, params.xPos, params.yPos, paletteIndex, objsTileBase+TARGET_OFS, thisLevel);
      thisLevel->addTarget((Target*)(*levelmap)[params.xPos][params.yPos]);
#else
      printf("constructing Target at (%d,%d)\n",params.xPos,params.yPos);
#endif
      break;
    case SPIKE:
#ifdef ARM9
      (*levelmap)[params.xPos][params.yPos] = new Spike(tilesBg, params.xPos, params.yPos, paletteIndex, objsTileBase+SPIKE_OFS);
      if(params.isGoal)
      {
        thisLevel->addGoalTile((*levelmap)[params.xPos][params.yPos]);
      }
#else
      printf("constructing Spike at (%d,%d)\n",params.xPos,params.yPos);
#endif
      break;
    case ANGLEBLOCKER:
    case ANGLEREFLECTOR:
    case SPINBLOCKER:
    case SPINREFLECTOR:
    case SPINMIRROR:
      constructAngleblock(params);
      if(params.isGoal)
      {
        thisLevel->addGoalTile((*levelmap)[params.xPos][params.yPos]);
      }
      break;
    default:
      parse_error("unknown type to construct");
  }
}

%}

DIGIT         [0-9]
ANGLE         ("n"|"nne"|"ne"|"ene"|"e"|"ese"|"se"|"sse"|"s"|"ssw"|"sw"|"wsw"|"w"|"wnw"|"nw"|"nnw")
DIRECTION     ("left"|"right"|"up"|"down")
CORNER        ("tl"|"tr"|"bl"|"br")
%%
"#"           { register int c; while ((c = yyinput()) != '\n' && c != EOF); unput('\n'); }
\n            { if(state == BASESTATE && paren == 0) { initConstruct(); ++num_lines; } else parse_error("unfinished line"); }
"!"           { if(state == BASESTATE && paren == 0) state = GLOBAL; else  parse_error("misplaced '!'"); } 
"palette"     { if(state == GLOBAL && paren == 0) state = PALETTE; else parse_error("misplaced 'palette'"); }
"gremlins"    { if(state == GLOBAL && paren == 0) {gremlins = true; state=BASESTATE;} else parse_error("misplaced 'gremlins'"); }
"("           { if(((state == BASESTATE || (state & (PALETTE|SUBTILES|TELEPORTER|ANGLEBLOCK|LASER|RECEIVER))) && paren == 0) ||
                             (state == REFLECT && paren == 1))
                  ++paren;
                else
                  parse_error("misplaced '('");
              }
","           { switch(state)
                {
                case BASESTATE:
                  if(paren == 1 && toConstruct.xPos >= 0) state = YPOS;
                  else parse_error("missing x position");
                  break;
                case SUBTILES:
                  if(paren == 1 && subtileIdx < 3) ++subtileIdx;
                  else parse_error("bad subtiles description");
                  break;
                default:
                  parse_error("misplaced ','");
                  break;
                }
              }
{DIGIT}+      { switch(state)
                {
                case PALETTE:
                  if(paren == 1 && paletteIndex == -1) paletteIndex = atoi(yytext);
                  else  parse_error("misplaced palette spec."); 
                  break;
                case BASESTATE:
                  if(paren == 1 && toConstruct.xPos == -1) toConstruct.xPos = atoi(yytext);
                  else parse_error("misplaced xPos");
                  break;
                case YPOS:
                  if(paren == 1 && toConstruct.yPos == -1) toConstruct.yPos = atoi(yytext);
                  else parse_error("misplaced yPos");
                  break;
                case TELEPORTER:
                  if(paren == 1 && toConstruct.teleportSet == -1)
                  {
                    toConstruct.teleportSet = atoi(yytext);
                    if(toConstruct.teleportSet > MAX_TELEPORTS) parse_error("teleport idx. too high");
                  }
                  else parse_error("misplaced teleport idx.");
                  break;
                default:
                  parse_error("misplaced digit");
                  break;
                }
              }
{ANGLE}         { if(state == ANGLEBLOCK && paren == 1) toConstruct.angle = txtToAngle(yytext);
                else parse_error("misplaced angle");
              }
{CORNER}       {
                 if(state == REFLECT && paren == 2) toConstruct.subtiles[subtileIdx].reflectiveSides |= txtToCorner(yytext);
                 else parse_error("misplaced corner");
               }
{DIRECTION}     { switch(state)
                {
                case RECEIVER:
                case LASER:
                  if(paren == 1 && toConstruct.direction == -1) toConstruct.direction = txtToDirection(yytext);
                  else parse_error("misplaced direction");
                  break;
                case REFLECT:
                  if(paren == 2) toConstruct.subtiles[subtileIdx].reflectiveSides |= txtToDirection(yytext);
                  else parse_error("misplaced direction");
                  break;
                case ANGLEBLOCK:
                  if(paren == 1)
                  {
                    if(toConstruct.direction == -1) toConstruct.direction = 0;
                    toConstruct.direction |= txtToDirection(yytext);
                  }
                  else parse_error("misplaced direction");
                  break;
                default:
                  parse_error("misplaced direction");
                  break;
                }
              }
")"           { switch(state)
                {
                case PALETTE:
                  if(paren == 1 && paletteIndex >= 0) state = BASESTATE;
                  else parse_error("misplaced ')'");
                  --paren;
                  break;
                case YPOS:
                  if(paren == 1 && toConstruct.yPos >= 0) state = TILESPEC;
                  else parse_error("misplaced ')'");
                  --paren;
                  break;
                case SUBTILES:
                  if(paren == 1 && subtileIdx == 3)
                  {
                    construct(toConstruct);
                    state = BASESTATE;
                    subtileIdx = -1;
                    --paren;
                  }
                  else parse_error("misplaced ')'");
                  break;
                case REFLECT:
                  if(paren == 2) state = SUBTILES;
                  else parse_error("misplaced ')'");
                  --paren;
                  break;
                case TELEPORTER:
                  if(paren == 1 && toConstruct.teleportSet >= 0)
                  {
                    construct(toConstruct);
                    state = BASESTATE;
                    --paren;
                  }
                  else parse_error("misplaced ')'");
                  break;
                case ANGLEBLOCK:
                  if(paren == 1 && toConstruct.angle >= 0)
                  {
                    construct(toConstruct);
                    state = BASESTATE;
                    --paren;
                  }
                  else parse_error("misplaced ')'");
                  break;
                case LASER:
                case RECEIVER:
                  if(paren == 1 && toConstruct.direction > 0)
                  {
                    construct(toConstruct);
                    state = BASESTATE;
                    --paren;
                  }
                  break;
                default:
                  parse_error("misplaced ')'");
                  break;
                }
              }
"subtiles"    { if(state == TILESPEC && paren == 0 && subtileIdx == -1)
                {
                  toConstruct.type = SUBTILES;
                  state = SUBTILES;
                  subtileIdx = 0;
                }
                else parse_error("misplaced 'subtiles'");
              }
"goal_subtiles"    { if(state == TILESPEC && paren == 0 && subtileIdx == -1)
                {
                  toConstruct.type = SUBTILES;
                  toConstruct.isGoal = true;
                  state = SUBTILES;
                  subtileIdx = 0;
                }
                else parse_error("misplaced 'subtiles'");
              }
"reflect"     { if(state == SUBTILES && paren == 1 && toConstruct.subtiles[subtileIdx].type == -1)
                {
                  toConstruct.subtiles[subtileIdx].type = REFLECT;
                  state = REFLECT;
                }
                else parse_error("misplaced 'reflect'");
              }
"block"       { if(state == SUBTILES && paren == 1 && toConstruct.subtiles[subtileIdx].type == -1)
                {
                  toConstruct.subtiles[subtileIdx].type = BLOCK;
                }
                else parse_error("misplaced 'block'");
              }
"blank"       { if(state == SUBTILES && paren == 1 && toConstruct.subtiles[subtileIdx].type == -1)
                {
                  toConstruct.subtiles[subtileIdx].type = BLANK;
                }
                else parse_error("misplaced 'blank'");
              }
"mirror"      { if(state == TILESPEC && paren == 0)
                {
                  toConstruct.type = MIRROR;
                  construct(toConstruct);
                  state = BASESTATE;
                }
                else parse_error("misplaced 'mirror'");
              }
"target"      { if(state == TILESPEC && paren == 0)
                {
                  toConstruct.type = TARGET;
                  construct(toConstruct);
                  state = BASESTATE;
                }
                else parse_error("misplaced 'target'");
              }
"random"      { if(state == TILESPEC && paren == 0)
                {
                  toConstruct.type = RANDOM;
                  construct(toConstruct);
                  state = BASESTATE;
                }
                else parse_error("misplaced 'random'");
              }
"spinblocker" { if(state == TILESPEC && paren == 0)
                {
                  toConstruct.type = SPINBLOCKER;
                  state = ANGLEBLOCK;
                }
                else parse_error("misplaced 'spinblocker'");
              }
"spinmirror"  { if(state == TILESPEC && paren == 0)
                {
                  toConstruct.type = SPINMIRROR;
                  state = ANGLEBLOCK;
                }
                else parse_error("misplaced 'spinmirror'");
              }
"spinreflector" { if(state == TILESPEC && paren == 0)
                  {
                    toConstruct.type = SPINREFLECTOR;
                    state = ANGLEBLOCK;
                  }
                  else parse_error("misplaced 'spinreflector'");
                }
"spike"       { if(state == TILESPEC && paren == 0)
                {
                  toConstruct.type = SPIKE;
                  construct(toConstruct);
                  state = BASESTATE;
                }
                else parse_error("misplaced 'spike'");
              }
"goal_spike"  { if(state == TILESPEC && paren == 0)
                {
                  toConstruct.type = SPIKE;
                  toConstruct.isGoal = true;
                  construct(toConstruct);
                  state = BASESTATE;
                }
                else parse_error("misplaced 'spike'");
              }
"goal_teleporter"  { if(state == TILESPEC && paren == 0)
                {
                  toConstruct.type = TELEPORTER;
                  toConstruct.isGoal = true;
                  state = TELEPORTER;
                }
                else parse_error("misplaced 'teleporter'");
              }
"teleporter"  { if(state == TILESPEC && paren == 0)
                {
                  toConstruct.type = TELEPORTER;
                  state = TELEPORTER;
                }
                else parse_error("misplaced 'teleporter'");
              }
"goal_angleblocker"  { if(state == TILESPEC && paren == 0)
                  {
                    toConstruct.type = ANGLEBLOCKER;
                    toConstruct.isGoal = true;
                    state = ANGLEBLOCK;
                  }
                  else parse_error("misplaced 'angleblocker'");
                }
"goal_anglereflector"  { if(state == TILESPEC && paren == 0)
                    {
                      toConstruct.type = ANGLEREFLECTOR;
                      toConstruct.isGoal = true;
                      state = ANGLEBLOCK;
                    }
                    else parse_error("misplaced 'anglereflector'");
                  }

"angleblocker"  { if(state == TILESPEC && paren == 0)
                  {
                    toConstruct.type = ANGLEBLOCKER;
                    state = ANGLEBLOCK;
                  }
                  else parse_error("misplaced 'angleblocker'");
                }
"anglereflector"  { if(state == TILESPEC && paren == 0)
                    {
                      toConstruct.type = ANGLEREFLECTOR;
                      state = ANGLEBLOCK;
                    }
                    else parse_error("misplaced 'anglereflector'");
                  }
"laser"       { if(state == TILESPEC && paren == 0)
                {
                  toConstruct.type = LASER;
                  state = LASER;
                }
                else parse_error("misplaced 'laser'");
              }
"receiver"    { if(state == TILESPEC && paren == 0)
                {
                  toConstruct.type = RECEIVER;
                  state = RECEIVER;
                }
                else parse_error("misplaced 'receiver'");
              }
[ \t\n\r]     /* skip whitespace */
.             { parse_error("Unknown character"); }
%%

#ifdef ARM9
bool readLevel(FILE* input, Level* lev, Tile* (*gameMap)[TILES_X][TILES_Y], LevelParams& levParams, int tilesBgHandle, int mirrorsBgHandle)
{
  for(int n = 0; n < MAX_TELEPORTS; ++n)
  {
    teleporters[n].first = NULL;
    teleporters[n].second = NULL;
  }
  initConstruct();
  paletteIndex = -1;
  tilesBg = tilesBgHandle;
  mirrorsBg = mirrorsBgHandle;
  levelmap = gameMap;
  yyrestart(input);
  thisLevel = lev;
  mirrorsTileBase = levParams.mirrorsTileBase;
  objsTileBase = levParams.objsTileBase;
  spinnersTileBase = levParams.spinnersTileBase;
  shipsTileBase = levParams.shipsTileBase;
  bool result;
  if(!setjmp(exception_env))
  {
    yylex();
    Settings::get().setPalIdx((unsigned int)paletteIndex);
    lev->setSpawnGremlins(gremlins);
    //TODO: check validity?
    result = true;
  }
  else
  {
    result = false;
  }
  return result;
}
#else
int main(int argc, char* argv[])
{
  printf("testing levelReader\n");
  if(argc > 1)
  {
    initConstruct();
    paletteIndex = -1;
    tilesBg = 1;
    mirrorsBg = 1;
    mirrorsTileBase = 0;
    objsTileBase = 0;
    spinnersTileBase = 0;
    FILE* fp = fopen(argv[1], "r");
    if(fp)
    {
      yyrestart(fp);
      if(!setjmp(exception_env))
      {
        yylex();
        //TODO: check validity?
        printf("success\n");
      }
      else
      {
        printf("failure\n");
      }
    }
  }
  return 0;
}
#endif
