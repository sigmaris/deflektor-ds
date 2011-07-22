/* ---------------------------------------------------------------------------------*/
#include <nds.h>
#include <stdio.h>
#include <math.h>
#include <fat.h>
#include <sys/dir.h>
#include <string.h>
#include <maxmod9.h>    // Maxmod definitions for ARM9
#include <list>

#include "filesystem.h"
#include "soundbank.h"  // Soundbank definitions
#include "soundbank_bin.h"
#include "level.h"
#include "levelset.h"
#include "settings.h"
#include "bar.h"
#include "star.h"

#include "stargfx.h"
#include "bars.h"
#include "title.h"
#include "gameover.h"
#include "levelselect_back.h"
#include "decor.h"
#include "mirrors.h"
#include "gremlins.h"
#include "objs.h"
#include "ships.h"
#include "spinreflectorgfx.h"
#include "spinblockergfx.h"
#include "palette.h"
#include "font.h"
#include "gamedisplay.h"

#define STARPALS 3
#define STARCOUNT 33

int gameBg;
int mirrorsBg;
int beamBg;
int decorBg;
int bmpBg;
int consoleBg;
int gamedisplayBg;
int stretchLine = 0;

bool oddFrame;
bool audio = false;

void vblankHandler(void)
{
  //flip oddFrame
  oddFrame ^= true;
}

void initGfx()
{
  //init backgrounds
  //This one holds the beam - a 16bpp bitmap 
  beamBg = bgInit(BEAMBG_IDX, BgType_Bmp16, BgSize_B16_256x256, 2, 0);
  
  //This one holds the game tiles and the moving parts of mirrors
  gameBg = bgInit(GAMEBG_IDX, BgType_Text4bpp, BgSize_T_256x256, 0,1);
  
  //This one holds the back part of the mirror so the beam can draw over it
  mirrorsBg = bgInit(MIRRORSBG_IDX, BgType_Text4bpp, BgSize_T_256x256, 1,1);
  
  //Holds the frame that goes around the edge of the screen
  decorBg = bgInit(DECORBG_IDX, BgType_Text4bpp, BgSize_T_256x256,2,1);
  
  //Holds the top screen score, lives and level display
  gamedisplayBg = bgInitSub(GAMEDISPLAYBG_IDX, BgType_Text4bpp, BgSize_T_256x256,4,0);

  Settings::get().setBgHandles((struct BgHandles){
                               beamBg:beamBg,
                               mirrorsBg:mirrorsBg,
                               gameBg:gameBg,
                               decorBg:decorBg,
                               gamedisplayBg:gamedisplayBg
                               });
  
  //init sprites
  oamInit(&oamMain, SpriteMapping_1D_64, false);
  
  Bar* energyBar = new Bar(coord(-42,150), (u16*)&barsTiles, 3);
  Bar* overloadBar = new Bar(coord(70,150), (u16*)(32+((char*)&barsTiles)), 3);
  
  //There are only 3 different types of gfx for gremlins
  u16* gremlinType1Gfx = oamAllocateGfx(&oamMain, SpriteSize_16x32, SpriteColorFormat_16Color);
  u16* gremlinType2Gfx = oamAllocateGfx(&oamMain, SpriteSize_16x32, SpriteColorFormat_16Color);
  u16* gremlinType3Gfx = oamAllocateGfx(&oamMain, SpriteSize_16x32, SpriteColorFormat_16Color);
  
  u16* starGfx = oamAllocateGfx(&oamMain, SpriteSize_8x8, SpriteColorFormat_16Color);

  Settings::get().setOverloadBar(overloadBar);
  Settings::get().setEnergyBar(energyBar);
    
  Settings::get().setSpriteGfx((struct SpriteGfx){gremlinType1Gfx,gremlinType2Gfx,gremlinType3Gfx, starGfx});
  
  //Arrange BG proprities, from top to bottom:
  bgSetPriority(gameBg, 0);
  bgSetPriority(beamBg, 2);
  bgSetPriority(mirrorsBg, 3);
  
  //Move the game BG offset slightly to center the game board within the frame
  bgSetScroll(gameBg, -XOFS, -YOFS);
  bgSetScroll(beamBg, 1-XOFS, 1-YOFS);
  bgSetScroll(mirrorsBg, -XOFS, -YOFS);
  bgUpdate();
}

void copyGfx()
{
  //Copy gfx data to VRAM
  
  LevelParams params;
  
  int bytesWritten = 32; //Skip tile position 0 in VRAM
  
  params.mirrorsTileBase = 1; // (bytesWritten) / 32;
  u8* bgGfxPtr = (u8*)bgGetGfxPtr(gameBg);
  dmaCopy(&mirrorsTiles, bytesWritten+bgGfxPtr, mirrorsTilesLen);
  bytesWritten += mirrorsTilesLen;
  
  params.shipsTileBase = (bytesWritten) / 32;
  dmaCopy(&shipsTiles, bytesWritten+bgGfxPtr, shipsTilesLen);
  bytesWritten += shipsTilesLen;
  
  params.objsTileBase = (bytesWritten) / 32;
  dmaCopy(&objsTiles, bytesWritten+bgGfxPtr, objsTilesLen);
  bytesWritten += objsTilesLen;
  
  params.spinnersTileBase = (bytesWritten) / 32;
  dmaCopy(&spinblockergfxTiles, bytesWritten+bgGfxPtr, spinblockergfxTilesLen);
  bytesWritten += spinblockergfxTilesLen;
  
  //unsigned int spinnersTileBase2 = (bytesWritten) / 32;
  dmaCopy(&spinreflectorgfxTiles, bytesWritten+bgGfxPtr, spinreflectorgfxTilesLen);
  bytesWritten += spinreflectorgfxTilesLen;
  dmaCopy(&decorTiles, bytesWritten+bgGfxPtr, decorTilesLen);
  bytesWritten += decorTilesLen;
  dmaCopy(&levelselect_backTiles, bytesWritten+bgGfxPtr, levelselect_backTilesLen);
  bytesWritten += levelselect_backTilesLen;
  dmaCopy(&fontTiles, bytesWritten+bgGfxPtr, fontTilesLen);
  Settings::get().setMainFontBase(bytesWritten / 32);
  bytesWritten += fontTilesLen;
  
  dmaCopy(&gamedisplayTiles, bgGetGfxPtr(gamedisplayBg) + 1520, gamedisplayTilesLen);
  
  dmaCopy(&decorMap, bgGetMapPtr(decorBg), decorMapLen);
  dmaCopy(&levelselect_backMap, BG_MAP_RAM(3), levelselect_backMapLen);
  dmaCopy(&gamedisplayMap, bgGetMapPtr(gamedisplayBg), gamedisplayMapLen);
  
  dmaCopy(&palettePal, BG_PALETTE, palettePalLen);
  dmaCopy(&gamedisplayPal, BG_PALETTE_SUB, gamedisplayPalLen);

  //Set palette's 0 to black instead of the purple we use in the GFX
  for(int p=0; p<256; p+=16)
  {
    BG_PALETTE[p] = 0;
    BG_PALETTE_SUB[p] = 0;
  }
  
  SpriteGfx& spr = Settings::get().getSpriteGfx();
  
  //copy Gremlin gfx
  dmaCopy(&gremlinsTiles, spr.gremlinType1Gfx, 256);
  dmaCopy(256+((char*)&gremlinsTiles), spr.gremlinType2Gfx, 256);
  dmaCopy(512+((char*)&gremlinsTiles), spr.gremlinType3Gfx, 256);
  
  //copy Star gfx
  dmaCopy(&stargfxTiles, spr.starGfx, 32);
  
  dmaCopy(&palettePal, SPRITE_PALETTE, palettePalLen);
  dmaCopy(&barsPal, 48+SPRITE_PALETTE, barsPalLen);
  dmaCopy(&gremlinsPal, 64+SPRITE_PALETTE, gremlinsPalLen);
  
  //Store recorded VRAM addresses in Settings global object
  Settings::get().setLevelParams(params);
}

//Does the stretch effect when the title screen appears
void stretchEffectHandler()
{
  REG_BG3Y_SUB = stretchLine << 8;
  irqEnable(IRQ_HBLANK);
}

//Prints some text on a tile BG at a given position
void printAt(int x, int y, int bgId, const char* string)
{
  u16* bgMapPtr = bgGetMapPtr(bgId);
  char c = *string;
  while(c)
  {
    bgMapPtr[Tile::pos2idx(x++, y)] = Settings::get().getMainFontBase() + c - 0x20;
    c = *(++string);
  }
}

//Shows the settings screen to the player. Takes control until the player starts the game
void showSettings()
{
  bgSetMapBase(decorBg, 4);
  dmaFillWords(0, bgGetMapPtr(decorBg), 2*32*32);
  bgShow(decorBg);
  bool exited = false;
  touchPosition touch;
  Settings& s = Settings::get();
  while(!exited)
  {
    scanKeys();
    if(keysDown() & KEY_TOUCH)
    {
      touchRead(&touch);
      switch(touch.py >> 3)
      {
        case 6:
          s.setMusicOn(s.getMusicOn() ^ true);
          if(s.getMusicOn()) mmResume();
          else mmPause();
          break;
        case 9:
          s.setSfxOn(s.getSfxOn() ^ true);
          break;
        case 12:
          s.setControlType(s.getControlType() ^ true);
          break;
        case 15:
          exited = true;
          break;
        default:
          break;
      }
    }
    
    swiWaitForVBlank();
    
    printAt(2, 2, decorBg, "Settings");
    printAt(2, 6, decorBg, "Music");
    printAt(8,6,decorBg,(Settings::get().getMusicOn() ? "On " : "Off"));
    
    printAt(2, 9, decorBg, "Sound FX");
    printAt(11,9,decorBg,(Settings::get().getSfxOn() ? "On " : "Off"));
    
    printAt(2, 12, decorBg, "Control");
    printAt(10,12,decorBg,(Settings::get().getControlType() ? "Touch+Drag " : "Tap+Keys  "));
    
    printAt(2, 15, decorBg, "Start");
  }
}

//Shows the title screen. Takes over control until the player quits, starts the game or shows the settings screen
bool showTitleScreen()
{
  videoSetModeSub(MODE_3_2D);
  //Clear level map
  LevelSet::blackScreen();
  bgShow(decorBg);
  bgShow(beamBg);
  bgSetMapBase(decorBg, 4);
  dmaFillWords(0, bgGetMapPtr(decorBg), 2*32*32);
  printAt(10, 2, decorBg, "Deflektor DS");
  printAt(2, 5, decorBg, "Original by Vortex Software");
  printAt(5, 6, decorBg, "DS Version by sigmaris");
  printAt(1, 15, decorBg, "Press Start or tap for new game");
  printAt(4, 16, decorBg, "Press Select for options");
  printAt(4, 17, decorBg, "Press L+R+Start to quit");
  
  //Copy title screen gfx to VRAM for top (sub) screen
  vramSetBankC(VRAM_C_SUB_BG);
  bmpBg = bgInitSub(BMPBG_IDX, BgType_Bmp16, BgSize_B16_256x256, 2, 0);
  dmaCopy(&titleBitmap, bgGetGfxPtr(bmpBg), titleBitmapLen);
  bgSetScroll(bmpBg, 0, 0);
  bgUpdate();
  
  //do the stretch title screen effect
  irqSet(IRQ_VCOUNT, stretchEffectHandler);
  irqSet(IRQ_HBLANK, stretchEffectHandler);
  irqEnable(IRQ_VCOUNT);
  for(stretchLine = 0; stretchLine < 192; ++stretchLine)
  {
    REG_BG3Y_SUB = 0;
    SetYtrigger(stretchLine);
    swiWaitForVBlank();
    irqDisable(IRQ_HBLANK);
    //Fade in the main screen on the last few lines
    if(stretchLine > 175) REG_BLDY = 192 - stretchLine;
  }
  irqDisable(IRQ_VCOUNT);
  irqClear(IRQ_VCOUNT);
  irqDisable(IRQ_HBLANK);
  irqClear(IRQ_HBLANK);
  REG_BG3Y_SUB = 0;
  LevelSet::disableFade();
  
  //Set alpha blending between decorBg and sprites
  REG_BLDCNT = (1 << (DECORBG_IDX + 8)) |
  //(1 << 13) | //backdrop
  //(1 << 12) | //sprite layer
  (1 << 6);
  REG_BLDALPHA = 12 | (8 << 8);
  
  Star* stars[STARPALS][STARCOUNT];
  for(int p=0; p<STARPALS; ++p)
    for(int i=0; i<STARCOUNT; ++i)
    {
      stars[p][i] = new Star(p);
      stars[p][i]->drawUpdate();
      stars[p][i]->setAlphaBlend(true);
    }
  swiWaitForVBlank();
  oamUpdate(&oamMain);
  
  scanKeys();
  int kd = keysDown();
  //capture 1 frame to use as a base for motion blur
  vramSetBankB(VRAM_B_LCD);
  REG_DISPCAPCNT = DCAP_ENABLE |
  DCAP_MODE(0) |
  DCAP_SRC(0) |
  DCAP_SIZE(3) |
  DCAP_OFFSET(0) |
  DCAP_BANK(1);

  for(int p=0; p<STARPALS; ++p)
    for(int i=0; i<STARCOUNT; ++i)
    {
      stars[p][i]->drawUpdate();
      stars[p][i]->setAlphaBlend(true);
    }
  swiWaitForVBlank();
  oamUpdate(&oamMain);
  
  //Set video mode manually for motion blur fx
  int oldmode = REG_DISPCNT;
  videoSetMode(0x60003 | DISPLAY_BG2_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D | DISPLAY_SPR_1D_SIZE_64);

  while(!(kd & (KEY_START | KEY_TOUCH | KEY_SELECT)))
  {
    for(int p=0; p<STARPALS; ++p)
      for(int i=0; i<STARCOUNT; ++i)
      {
        stars[p][i]->drawUpdate();
        stars[p][i]->setAlphaBlend(true);
      }
    swiWaitForVBlank();
    oamUpdate(&oamMain);
    
    //Capture screen and blend with existing contents of VRAM_B
    REG_DISPCAPCNT = DCAP_ENABLE |
    DCAP_MODE(2) |
    DCAP_SRC(0) |
    DCAP_SIZE(3) |
    DCAP_OFFSET(0) |
    DCAP_A(4) |
    DCAP_B(14) |
    DCAP_BANK(1);

    scanKeys();
    kd = keysDown();
  }
  for(int p=0; p<STARPALS; ++p)
    for(int i=0; i<STARCOUNT; ++i)
      delete stars[p][i];
  
  oamUpdate(&oamMain);
  REG_BLDCNT = 0;
  REG_BLDALPHA = 0;
  videoSetMode(oldmode);
  bgUpdate();

  if(((kd | keysHeld()) & (KEY_L | KEY_R | KEY_START)) == (KEY_L | KEY_R | KEY_START))
    return false;
  else if(kd & KEY_SELECT) 
    showSettings();

    return true;
}

//Shows the screen with a list of level sets for the user to choose from and
//waits for them to tap & select one.
void chooseLevelSet(char* dir)
{
  bgShow(decorBg);
  bgSetMapBase(decorBg, 3);
  printAt(8, 2, decorBg, "Select A Levelset");
  bgUpdate();
  char* dirnames[17];
  for (int i = 0; i < 17; ++i)
  {
    dirnames[i] = NULL;
  }
  int dirlen = strlen(dir);
  DIR* dp = opendir(dir);
  if(dp)
  {
    int yPos = 5;
    struct dirent* nextdir;
    while((nextdir = readdir(dp)) && yPos <= 21)
    {
      if(nextdir->d_name[0] != '.')
      {
        char* dirname = (char*)malloc(strlen(nextdir->d_name) + 1);
        strcpy(dirname, nextdir->d_name);
        dirnames[yPos-5] = dirname;
        printAt(2, yPos++, decorBg, dirname);
      }
    }
    bool selected = false;
    while(!selected)
    {
      swiWaitForVBlank();
      scanKeys();
      if(keysDown() & KEY_TOUCH)
      {
        touchPosition touchDown;
        touchRead(&touchDown);
        if(touchDown.px > 16 && touchDown.px < 240 && touchDown.py > 40 && touchDown.py < 176)
        {
          int selIndex = (touchDown.py - 40) >> 3;
          if(dirnames[selIndex])
          {
            strcpy(dir+dirlen, dirnames[selIndex]);
            selected = true;
          }
        }
      }
    }
    //Clean up
    for (int i = 0; i < 17; ++i)
    {
      if(dirnames[i]) free(dirnames[i]);
    }
    closedir(dp);
  }
  bgSetMapBase(decorBg, 2);
}

void initConsole()
{
  consoleBg = bgInitSub(CONSOLEBG_IDX, BgType_Text4bpp, BgSize_T_256x256, 3, 0);
  PrintConsole *console = consoleInit(NULL, 0, BgType_Text4bpp, BgSize_T_256x256, 3,0, false, true);
  
  ConsoleFont font;
  
  font.gfx = (u16*)fontTiles;
  font.pal = (u16*)palettePal;
  font.numChars = 95;
  font.numColors = palettePalLen / 2;
  font.bpp = 4;
  font.asciiOffset = 32;
  font.convertSingleColor = false;
  
  consoleSetFont(console, &font);  //setup the sub screen for printing
  consoleDebugInit(DebugDevice_CONSOLE);
  
}

int main(void) {
  powerOn(POWER_ALL_2D);
  
  lcdMainOnBottom();
  
  irqSet(IRQ_VBLANK, vblankHandler);
  
  videoSetMode(MODE_3_2D);
  
  vramSetBankA(VRAM_A_MAIN_BG);
  //vramSetBankB(VRAM_B_MAIN_BG);
  vramSetBankC(VRAM_C_SUB_BG);
  vramSetBankE(VRAM_E_MAIN_SPRITE);
  
  initConsole();
  
  //check if we're running in an (inaccurate) emulator
  vramSetBankD(VRAM_D_LCD);
  VRAM_D[0] = 0;
  char* vramAsChar = (char*)VRAM_D;
  vramAsChar[0] = 0x34;
  vramAsChar[1] = 0x12;
  
  if(VRAM_D[0] != 0x1234)
  {
    //init sound - breaks in DesMuMe so don't do it in emulator
    iprintf("playing music\n");
    audio = true;
    mmInitDefaultMem((mm_addr)soundbank_bin);
    mmLoad(MOD_WIRE_DEFLECTOR);
    mmLoadEffect(SFX_EXPLODE);
    mmLoadEffect(SFX_GREMLIN);
    mmLoadEffect(SFX_GREMHIT);
    mmLoadEffect(SFX_GREMDIE);
    mmLoadEffect(SFX_LEVELUP);
    mmStart(MOD_WIRE_DEFLECTOR, MM_PLAY_LOOP);
  }
  else
  {
    iprintf("not playing music\n");
    audio = false;
  }

  //Set up graphics and copy bitmaps into VRAM
  initGfx();
  copyGfx();
  
  if(!showTitleScreen()) return 0;
  
  bgShow(bmpBg);
  bgHide(consoleBg);
  bgUpdate();
  
#ifndef FAT
  iprintf("Initialising NitroFS\n");
  bool nitroInit = false;
  if(!nitroFSInit())
  {
    __system_argv->argvMagic = ARGV_MAGIC; //nasty hack to make it work in emulator!
    __system_argv->argc = 1;
    __system_argv->argv[0] = "fat:/deflektor-ds.nds";
    if(nitroFSInit())
    {
      nitroInit = true;
    }
  }
  else
  {
    nitroInit = true;
  }
  if(nitroInit)
#else
  iprintf("Initialising FAT\n");
  if(fatInitDefault()==1)
#endif
  {
    iprintf("FS init success\n");

    chdir("/");
    //Play level set
    char dirname[MAXPATHLEN];
    bool running = true;
    while(running)
    {
      strcpy(dirname,"data/dflektor/levels/");
      chooseLevelSet(dirname);
      LevelSet* ls = new LevelSet(dirname);
      ls->playLevelSet();
      
      delete ls;
      running = showTitleScreen();
    }
  }
  else 
  {
#ifndef FAT
    iprintf("nitroFS init failed. Did you DLDI patch correctly?\n");
#else
    iprintf("FAT init failed. Did you DLDI patch correctly?\n");
#endif
    // Pause for a while...
    for(int i = 0; i < 240; ++i)
    {
      swiWaitForVBlank();
    }
    return 1;
  }
  
	return 0;
}


