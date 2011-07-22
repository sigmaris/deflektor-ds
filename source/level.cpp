#include <stdio.h>
#include <sys/dir.h>
#include <maxmod9.h>    // Maxmod definitions for ARM9

#include <utility>
#include <list>

#include "soundbank.h"
#include "tile.h"
#include "mirror.h"
#include "spinmirror.h"
#include "spinblocker.h"
#include "spinreflector.h"
#include "teleporter.h"
#include "laser.h"
#include "receiver.h"
#include "beam.h"
#include "spike.h"
#include "random.h"
#include "target.h"
#include "settings.h"
#include "subtile.h"
#include "subtileholder.h"
#include "gremlin.h"
#include "bar.h"

#include "level.h"

//Parses and loads a level from a text file
extern bool readLevel(FILE* input, Level* lev, Tile* (*gameMap)[TILES_X][TILES_Y], LevelParams& levParams, int tilesBgHandle, int mirrorsBgHandle);
extern bool oddFrame; //Is this an odd or even frame?
extern bool audio; //Should we try to use audio?
extern int consoleBg;
extern int bmpBg;

using namespace std;
typedef pair<coord,bool> pathPoint;

Level::Level(char* path,
             int tilesBg,
             int mirrorsBg,
             int beamBg,
             LevelParams& params) : 
tilesBg(tilesBg),
mirrorsBg(mirrorsBg),
beamBg(beamBg),
mirrorsTileBase(params.mirrorsTileBase),
shipsTileBase(params.shipsTileBase),
objsTileBase(params.objsTileBase),
spinnersTileBase(params.spinnersTileBase),
toAnimate(),
targets(),
goalTiles(),
gremlins(),
beam(beamBg, 0xFFFF),
blankTile(new Tile(tilesBg, 0, 0, 0)),
complete(false),
activeMirror(NULL),
energy(BAR_MAX),
overload(0),
score(0)
{
  for(int i = 0; i < TILES_X; ++i)
  {
    for(int j = 0; j < TILES_Y; ++j)
    {
      gameMap[i][j] = blankTile;
    }
  }
  //Try and read level data from a file
  FILE* test = fopen(path, "r");
  if(test)
  {
    if(readLevel(test, this, &gameMap, params, tilesBg, mirrorsBg))
    {
      iprintf("Parsing level: Success\n");
    }
    else
    {
      iprintf("Parsing level: Failure");
    }
  }
  else
  {
    iprintf("Parsing level: couldn't open file\n");
  }
  fclose(test);
}

Level::~Level()
{
  for(int i=0; i < TILES_X; i++)
  {
    for(int j=0; j < TILES_Y; j++)
    {
      if(gameMap[i][j] != blankTile)
	    {
	      delete gameMap[i][j];
	    }
    }
  }
  delete blankTile;
  
  list<Gremlin*>::iterator gremlinIter;
  for(gremlinIter = gremlins.begin();
      gremlinIter != gremlins.end();
      ++gremlinIter)
  {
    delete (*gremlinIter);
  }
}

void Level::setSpawnGremlins(bool spawn)
{
  spawnGremlins = spawn;
}

void Level::setSource(Laser* src)
{
  if(src)
  {
    source = src;
  }
}

void Level::setReceiver(Receiver* rec)
{
  if(rec)
  {
    sink = rec;
  }
}

void Level::spawnGremlin()
{
  int gCount = gremlins.size();
  if(gCount < MAX_GREMLINS)
  {    
    gremlins.push_back(new Gremlin(this));
    iprintf("spawned gremlin %d\n",gCount);
    if(Settings::get().getSfxOn()) mmEffect(SFX_GREMLIN);
  }
}

void Level::addAnimatedTile(Tile* toAdd)
{
  if(toAdd) toAnimate.push_back(toAdd);
}

void Level::addGoalTile(Tile* toAdd)
{
  if(toAdd) goalTiles.push_back(toAdd);
}

Tile* Level::getTileAt(unsigned int x, unsigned int y)
{
  return gameMap[x][y];
}

//Traces the path of the laser beam, once per frame, and draws it on the beam BG
//Returns 0 if everything is fine or a positive number if the beam overloaded (hit a spike, etc)
//Higher return values mean worse overload
int Level::evaluateBeam()
{
  beam.clear();
  
  BeamResult start = source->getBeamStart();
  
  bool curFrame = oddFrame;
  
  bool blocked = false;
  bool overloaded = false;
  int overloadFlag = 0;
  pair<unsigned int, unsigned int> nextPos;
  unsigned int nextXPos;
  unsigned int beamStartX = start.xPos;
  unsigned int nextYPos;
  unsigned int beamStartY = start.yPos;
  int nextXTile;
  int nextYTile;
  
  //curFrame != oddFrame is a signal that we spent too long evaluatingBeam
  //and the next frame is here already
  while (!blocked && curFrame == oddFrame)
  {
    nextPos = Beam::calculateNextPos(start.xPos, start.yPos, start.direction);
    nextXPos = nextPos.first;
    nextYPos = nextPos.second;
    if(nextXPos >= (TILES_X * 4) || nextXPos <= 0 || nextYPos >= (TILES_Y * 4) || nextYPos <= 0)
    {
      blocked = true;
      beam.draw(beamStartX, beamStartY, nextXPos, nextYPos);
    }
    else
    {
      nextXTile = nextXPos / 4;
      nextYTile = nextYPos / 4;
      if(nextXPos % 4 == 0)
      {
        //Are we going west in any direction?
        if(start.direction > S)
        {
          nextXTile = nextXPos / 4 - 1;
        }
      }
      if(nextYPos % 4 == 0)
      {
        //Are we going north in any direction?
        if(start.direction < E || start.direction > W)
        {
          nextYTile = nextYPos / 4 - 1;
        }
      }
      
      BeamResult res;
      res.action = Pass;
      
      //Find what happens when the beam enters the next tile:
      if(gameMap[nextXTile][nextYTile] != blankTile)
        res = gameMap[nextXTile][nextYTile]->beamEnters(nextXPos, nextYPos, start.direction);
      
      switch (res.action) {
        case Pass:
          start.xPos = nextXPos;
          start.yPos = nextYPos;
          //beam->drawBox(nextXTile*16, nextYTile*16, nextXTile*16+16, nextYTile*16+16,0xFFFF);
          break;
        case Block:
          blocked = true;
          beam.draw(beamStartX, beamStartY, nextXPos, nextYPos);
          //beam->drawBox(nextXTile*16, nextYTile*16, nextXTile*16+16, nextYTile*16+16,0x8FF0);
          break;
        case Reflect:
          start.xPos = res.xPos;
          start.yPos = res.yPos;
          start.direction = res.direction;
          beam.draw(beamStartX, beamStartY, res.xPos, res.yPos);
          beamStartX = res.xPos;
          beamStartY = res.yPos;
          //beam->drawBox(nextXTile*16, nextYTile*16, nextXTile*16+16, nextYTile*16+16,0xF000);
          break;
        case Teleport:
          beam.draw(beamStartX, beamStartY, nextXPos, nextYPos);
          start.xPos = res.xPos;
          start.yPos = res.yPos;
          start.direction = res.direction;
          beamStartX = res.xPos;
          beamStartY = res.yPos;
          break;
        case Overload:
          blocked = true;
          overloaded = true;
          overloadFlag = res.flag;
          beam.draw(beamStartX, beamStartY, nextXPos, nextYPos);
          //beam->drawBox(nextXTile*16, nextYTile*16, nextXTile*16+16, nextYTile*16+16, 0x801F);
          break;
        case Complete:
          complete = true;
          blocked = true;
          beam.draw(beamStartX, beamStartY, nextXPos, nextYPos);
          iprintf("level complete!\n");
          break;
      }
      
    }
  }
  
  return (overloaded ? overloadFlag : 0);
}

void Level::addTarget(Target* t)
{
  if(t)
  {
    targets.insert(t);
  }
}

//Called from Target when it is completely destroyed
void Level::targetDestroyed(Target* t)
{
  if(t)
  {
    targets.erase(t);
    score += (energy >> 1);
    if(targets.empty())
    {
      list<Tile*>::iterator goalsIter;
      for(goalsIter = goalTiles.begin();
          goalsIter != goalTiles.end();
          ++goalsIter)
      {
        (*goalsIter)->destroy();
        iprintf("destroyed %p\n",(*goalsIter));
      }
    }
    iprintf("destroyed %p, %d left\n",t,targets.size());
  }
}

void Level::printSub(int x, int y, int bgId, int palIdx, char* string)
{
  u16* bgMapPtr = bgGetMapPtr(bgId);
  char c = *string;
  while(c)
  {
    bgMapPtr[Tile::pos2idx(x++, y)] = (c - 0x20) | (palIdx << 12);
    c = *(++string);
  }
}

//Called when a Target is hit by the beam
void Level::targetHit(Target* t)
{
  iprintf("target %p hit\n",t);
  //Start animating this Target's explosion
  if(t)
  {
    toAnimate.push_back(t);
    if(Settings::get().getSfxOn()) mmEffect(SFX_EXPLODE);
  }
}

bool Level::isComplete()
{
  return complete;
}

//Called to play the level, does not return until level ends
//Returns -1 if the player 'died', -2 if they quit, or a
//positive score if the player completed the level
int Level::playLevel(int startScore)
{
  int energyCountdown = 0;
  int overloadFlag = 0;
  //Save the player's starting score
  score = startScore;
  scanKeys();
  
  int kd,kh;
  
  Bar* energyBar = Settings::get().getEnergyBar();
  Bar* overloadBar = Settings::get().getOverloadBar();
  int gamedisplayBg = Settings::get().getBgHandles().gamedisplayBg;
  energyBar->show();
  overloadBar->show();
  
  bgHide(consoleBg);
  bgHide(bmpBg);
  bgShow(gamedisplayBg);
  
  //main level loop
  while(!isComplete())
  {
    swiWaitForVBlank();
    
    scanKeys();
    
    kd = keysDown();
    kh = keysHeld();
    
    energyCountdown++;
    //Decrease energy every 54 frames (empirically determined ;)
    if(energyCountdown > 54)
    {
      energyCountdown = 0;
      energy--;
      if(energy <= 0)
      {
        energy = 0;
        return -1; //end level here
      }
    }
    
    overloadFlag = evaluateBeam();
    if(overloadFlag && oddFrame)
    {
      overload+=overloadFlag;
      if(overload > BAR_MAX)
      {
        overload = BAR_MAX;
        //need to end level here
        return -1;
      }
    }
    else if(overload && oddFrame)
    {
      //Overload bar gradually returns to normal
      --overload;
    }
    
    energyBar->setValue(energy);
    overloadBar->setValue(overload);
    oamUpdate(&oamMain);
    
    //if(!((kd | kh) & KEY_L))
    //{
    //  //show debug console
    //  bgHide(consoleBg);
    //  bgHide(bmpBg);
    //  bgShow(gamedisplayBg);
    //}
    //else
    //{
    //  bgHide(bmpBg);
    //  bgHide(gamedisplayBg);
    //  bgShow(consoleBg);
    //}
    
    if(spawnGremlins && oddFrame && (rand() & 0x1FF) == 0x1FF)
    {
      spawnGremlin();
    }
    
    if(kd & KEY_TOUCH)
    {
      touchRead(&touchDown);
    }
    
    if((kd & KEY_SELECT) && audio)
    {
      //toggle music
      Settings& s = Settings::get();
      s.setMusicOn(s.getMusicOn() ^ true);
      if(s.getMusicOn()) mmResume();
      else mmPause();
    }
    else if(kd & KEY_START)
    {
      //pause
      printSub(5, 20, gamedisplayBg, 0, "Press Start to Unpause");
      printSub(6, 21, gamedisplayBg, 0, "Press Select to Quit");
      while (true)
      {
        swiWaitForVBlank();
        scanKeys();
        kd = keysDown();
        kh = keysHeld();
        if(kd & KEY_START)
        {
          break;
        }
        else if(kd & KEY_SELECT)
        {
          printSub(5, 20, gamedisplayBg, 0, "                      ");
          printSub(6, 21, gamedisplayBg, 0, "                     ");
          return -2;
        }
      }
      printSub(5, 20, gamedisplayBg, 0, "                      ");
      printSub(6, 21, gamedisplayBg, 0, "                     ");
    }
    
    /*if(kd & KEY_A)
    {
      complete = true;
    }*/
    
    if(spawnGremlins)
    {
      list<Gremlin*>::iterator gremlinIter;
      for(gremlinIter = gremlins.begin();
          gremlinIter != gremlins.end();
          ++gremlinIter)
      {
        (*gremlinIter)->drawUpdate();
        if(kd & KEY_TOUCH)
        {
          coord gremlinPos = (*gremlinIter)->getPosition();
          if(touchDown.px > (gremlinPos.first) &&
             touchDown.px < (gremlinPos.first + 16) &&
             touchDown.py > (gremlinPos.second) &&
             touchDown.py < (gremlinPos.second + 16))
          {
            iprintf("tap on gremlin %p\n",*gremlinIter);
            if((*gremlinIter)->tapOn())
            {
              delete (*gremlinIter);
              gremlinIter = gremlins.erase(gremlinIter);
              if(Settings::get().getSfxOn()) mmEffect(SFX_GREMDIE);
            }
            else
            {
              if(Settings::get().getSfxOn()) mmEffect(SFX_GREMHIT);
            }
          }
        }
      }
    }
    
    list<Tile*>::iterator animateIter;
    for(animateIter = toAnimate.begin();
        animateIter != toAnimate.end();
        ++animateIter)
    {
      (*animateIter)->drawUpdate();
    }
    
    snprintf(scoreStr, 15, "SCORE: %07d",score);
    printSub(9, 7, gamedisplayBg, 1, scoreStr);
    
    //if(kd & KEY_DOWN) return -1;
    if(activeMirror && !Settings::get().getControlType())
    {
      if(kd & (KEY_LEFT | KEY_Y))
      {
        activeMirror->rotate(-1);
        activeMirror->drawUpdate();
      }
      else if(kd & (KEY_RIGHT | KEY_A))
      {
        activeMirror->rotate(1);
        activeMirror->drawUpdate();
      }
    }
    
    if(kd & KEY_TOUCH)
    {
      lastTouch = touchDown;
      int gameXpos = (touchDown.px - XOFS);
      int gameYpos = (touchDown.py - YOFS);
      if(gameXpos < 0 || gameXpos >= 240) continue; else gameXpos = gameXpos >> 4;
      if(gameYpos < 0 || gameYpos >= 144) continue; else gameYpos = gameYpos >> 4;
      Tile* tileAtPos = getTileAt(gameXpos, gameYpos);
      if(tileAtPos != NULL && tileAtPos->isInteractive())
      {
        //Assumes mirrors are the only interactive components - may change
        activeMirror = (Mirror*)tileAtPos;
      }
      else
      {
        activeMirror = NULL;
      }
    }
    else if(kh & KEY_TOUCH)
    {
      if(activeMirror && Settings::get().getControlType())
      {
        touchRead(&curTouch);
        //average current and last touch pos to smooth out 'jumping'
        int dx = ((lastTouch.px + curTouch.px) >> 1) - touchDown.px;
        int dy = ((lastTouch.py + curTouch.py) >> 1) - touchDown.py;
        lastTouch = curTouch;
        if(dx ==0 && dy == 0)
        {
          //do nothing
        }
        else if(dx == 0)
        {
          activeMirror->setAngle(0);
        }
        else if(dy == 0)
        {
          activeMirror->setAngle(8);
        }
        else
        {
          int a_dx = abs(dx);
          int a_dy = abs(dy);
          //Find magnitude of distance from mirror to touch point
          int32 magnitude = sqrtf32(inttof32(a_dx * a_dx + a_dy * a_dy));
          //use dot product to find cos. of angle between them
          int32 cos_angle = divf32(inttof32(-dy), magnitude);
          //work out the angle and correct for relative x position
          int angle = div32(((int)(acosLerp(cos_angle))),1024);
          if(dx < 0)
          {
            angle = 15 - angle;
          }
          activeMirror->setAngle(angle);
        }
        activeMirror->drawUpdate();
      }
    }
    else if(activeMirror && (keysUp() & KEY_TOUCH))
    {
      if(Settings::get().getControlType())
        activeMirror = NULL;
    }
    
    if(activeMirror && !Settings::get().getControlType())
    {
      coord pos = activeMirror->getPosition();
      beam.drawBox(pos.first*16, pos.second*16, (pos.first+1)*16, (pos.second+1)*16, 0xFFFF); 
    }
  }
  if(Settings::get().getSfxOn()) mmEffect(SFX_LEVELUP);
  for (int i=0; i < 66; ++i)
  {
    swiWaitForVBlank();
  }
  return score;
}