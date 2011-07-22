/*
 *  level.h
 *  deflektor-ds
 *
 *  Created by Hugh Cole-Baker on 01/12/2008.
 *  The Level manages one game level while it's being played.
 *
 */
#ifndef deflektor_level_h
#define deflektor_level_h

#include <list>
#include <set>
#include <nds.h>
#include "tile.h"

class Subtile;
class Target;
class Laser;
class Receiver;
class Beam;
class LevelParams;
class Mirror;
class Gremlin;

class Level
{
protected:
  static const u16 energyColor, overloadColor, blankColor; 
  const int tilesBg, mirrorsBg, beamBg;
  const unsigned int mirrorsTileBase, shipsTileBase, objsTileBase, spinnersTileBase;
  bool spawnGremlins;
  char scoreStr[15];
  
  //A list of tiles that should be animated (drawUpdate() called per frame)
  std::list<Tile*> toAnimate;
  
  //A set of targets the player needs to hit with the beam
  std::set<Target*> targets;
  
  //Once all targets are hit these tiles should be destroy()ed
  std::list<Tile*> goalTiles;
  
  //Manages any gremlins which have appeared on screen
  std::list<Gremlin*> gremlins;

  //The source of the beam
  Laser* source;
  
  //Where the player is trying to send the beam
  Receiver* sink;
  
  //The beam object
  Beam beam;
  
  //Array of tiles representing the game map
  Tile* gameMap[TILES_X][TILES_Y];
  
  //A shared blank tile instance to fill blanks in gameMap
  Tile* blankTile;

  //is the level complete?
  bool complete;
  
  //manages touch interaction with the tiles
  touchPosition touchDown;
  touchPosition lastTouch;
  touchPosition curTouch;
  Mirror* activeMirror;
  
  //The time, overload bar levels and the player's score for this level
  unsigned int energy, overload, score;
  
  void spawnGremlin();
public:
  Level(char* path,
        int tilesBg,
		 	  int mirrorsBg,
		 	  int beamBg,
		 	  LevelParams& params);
  virtual ~Level();
  Tile* getTileAt(unsigned int x, unsigned int y);
  int evaluateBeam();
  void targetDestroyed(Target* t);
  void targetHit(Target* t);
  void printSub(int x, int y, int bgId, int palIdx, char* string);
  void setSource(Laser* src);
  void setReceiver(Receiver* rec);
  void setSpawnGremlins(bool spawn);
  void addAnimatedTile(Tile* toAdd);
  void addTarget(Target* toAdd);
  void addGoalTile(Tile* toAdd);
  bool isComplete();
  int playLevel(int startScore);
};

#endif