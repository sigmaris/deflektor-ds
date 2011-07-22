/*
 *  settings.h
 *  deflektor-ds
 *
 *  Created by Hugh Cole-Baker on 4/4/09.
 *
 */

#ifndef deflektor_settings_h
#define deflektor_settings_h

#include <nds.h>

#define XOFS 8
#define YOFS 8

#define BAR_MAX 199

#define NUM_SPRITES 10
#define MAX_GREMLINS 3

#define BEAMBG_IDX 3
#define DECORBG_IDX 2
#define GAMEBG_IDX 1
#define MIRRORSBG_IDX 0

#define BMPBG_IDX 3
#define GAMEDISPLAYBG_IDX 1
#define CONSOLEBG_IDX 0

#define FRAMES_PER_SPRITE_FRAME 6

struct LevelParams {
  unsigned int mirrorsTileBase;
  unsigned int objsTileBase;
  unsigned int shipsTileBase;
  unsigned int spinnersTileBase;
};

struct SpriteGfx {
  u16* gremlinType1Gfx;
  u16* gremlinType2Gfx;
  u16* gremlinType3Gfx;
  u16* starGfx;
};

struct BgHandles {
  int beamBg;
  int mirrorsBg;
  int gameBg;
  int decorBg;
  int gamedisplayBg;
};

class Bar;

class Settings {
private:
  static Settings* instance;
  
  LevelParams params;
  BgHandles handles;
  SpriteGfx spriteGfx;
  unsigned int palIndex;
  unsigned int mainFontBase;
  Bar* energyBar;
  Bar* overloadBar;
  bool musicOn, sfxOn, controlType;
public:
  static Settings& get();
  
  Settings();
  
  LevelParams& getLevelParams();
  BgHandles& getBgHandles();
  SpriteGfx& getSpriteGfx();
  int getPalIdx();
  unsigned int getMainFontBase();
  Bar* getEnergyBar();
  Bar* getOverloadBar();
  bool getMusicOn();
  bool getSfxOn();
  bool getControlType();
  
  void setLevelParams(LevelParams params);
  void setBgHandles(BgHandles h);
  void setSpriteGfx(SpriteGfx gfx);
  void setPalIdx(unsigned int palIdx);
  void setMainFontBase(unsigned int fontBase);
  void setEnergyBar(Bar* ebar);
  void setOverloadBar(Bar* obar);
  void setMusicOn(bool value);
  void setSfxOn(bool value);
  void setControlType(bool type);
};

#endif