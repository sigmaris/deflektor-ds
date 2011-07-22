/*
 *  levelset.cpp
 *  deflektor-ds
 *
 *  Created by Hugh Cole-Baker on 4/1/09.
 *
 */

#include <nds.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <fat.h>
#include <sys/dir.h>
#include <string.h>
#include <errno.h>

#include "levelset.h"
#include "level.h"
#include "settings.h"
#include "sprite.h"
#include "gameover.h"
#include "bar.h"

void timerHandler(void)
{
  //this does nothing
}

void LevelSet::fadeToBlack()
{
  REG_BLDCNT = (1 << BEAMBG_IDX) | 
  (1 << GAMEBG_IDX) |
  (1 << MIRRORSBG_IDX) |
  (1 << DECORBG_IDX) |
  (1 << 4) | //sprite layer
  (3 << 6);
  for(int blend = 0; blend <= 16; ++blend)
  {
    swiWaitForVBlank();
    REG_BLDY = blend;
    swiWaitForVBlank();
  }
}

void LevelSet::fadeFromBlack()
{
  REG_BLDCNT = (1 << BEAMBG_IDX) | 
  (1 << GAMEBG_IDX) |
  (1 << MIRRORSBG_IDX) |
  (1 << DECORBG_IDX) |
  (1 << 4) | //sprite layer
  (3 << 6);
  for(int blend = 16; blend >= 0; --blend)
  {
    swiWaitForVBlank();
    REG_BLDY = blend;
    swiWaitForVBlank();
  }
}

void LevelSet::blackScreen()
{
  REG_BLDCNT = (1 << BEAMBG_IDX) | 
  (1 << GAMEBG_IDX) |
  (1 << MIRRORSBG_IDX) |
  (1 << DECORBG_IDX) |
  (1 << 4) | //sprite layer
  (3 << 6);
  REG_BLDY = 16;
}

void LevelSet::blackScreenSub()
{
  REG_BLDCNT_SUB = (1 << BMPBG_IDX) | 
  (1 << CONSOLEBG_IDX) |
  (3 << 6);
  REG_BLDY_SUB = 16;
}

void LevelSet::fadeToBlackSub()
{
  REG_BLDCNT_SUB = (1 << BMPBG_IDX) | 
  (1 << CONSOLEBG_IDX) |
  (3 << 6);
  for(int blend = 0; blend <= 16; ++blend)
  {
    swiWaitForVBlank();
    REG_BLDY_SUB = blend;
    swiWaitForVBlank();
  }
}

void LevelSet::disableFade()
{
  REG_BLDCNT = 0;
  REG_BLDY = 0;
}

void LevelSet::disableFadeSub()
{
  REG_BLDCNT_SUB = 0;
  REG_BLDY_SUB = 0;
}

void LevelSet::captureScreen(int bank)
{
  REG_DISPCAPCNT = DCAP_ENABLE |
  DCAP_MODE(0) |
  DCAP_SRC(0) |
  DCAP_SIZE(3) |
  DCAP_OFFSET(0) |
  DCAP_BANK(bank);
  while(REG_DISPCAPCNT & DCAP_ENABLE);
}

void LevelSet::blurScreen(int iters, int bank, bool fadeSub)
{
  if(fadeSub)
    REG_BLDCNT_SUB = (1 << BMPBG_IDX) | 
    (1 << CONSOLEBG_IDX) |
    (3 << 6);
  
  u16* vramBank = ((u16*)(0x6800000 + bank*0x20000));
  for(int times = 0; times < iters; ++times)
  {
    swiWaitForVBlank();
    if(fadeSub) REG_BLDY_SUB = times;
    /*int bAvg = ((vramBank[0] & 0x7c00) + (vramBank[1] & 0x7c00) + (vramBank[256] & 0x7c00) + (vramBank[257] & 0x7c00)) >> 10;
    int gAvg = ((vramBank[0] & 0x3e0) + (vramBank[1] & 0x3e0) + (vramBank[256] & 0x3e0) + (vramBank[257] & 0x3e0)) >> 5;
    int rAvg = ((vramBank[0] & 0x1f) + (vramBank[1] & 0x1f) + (vramBank[256] & 0x1f) + (vramBank[257] & 0x1f));
    for(int i=0; i < 48896; ++i)
    {
      vramBank[i] = ARGB16(1,(rAvg >> 2) & 0x1f,(gAvg >> 2) & 0x1f,(bAvg >> 2) & 0x1f);
      bAvg += (((vramBank[i+1] & 0x7c00) + (vramBank[i+257] & 0x7c00)) >> 10);
      bAvg -= (((vramBank[i] & 0x7c00) + (vramBank[i+256] & 0x7c00)) >> 10);
      gAvg += (((vramBank[i+1] & 0x3e0) + (vramBank[i+257] & 0x3e0)) >> 5);
      gAvg -= (((vramBank[i] & 0x3e0) + (vramBank[i+256] & 0x3e0)) >> 5);
      rAvg += ((vramBank[i+1] & 0x1f) + (vramBank[i+257] & 0x1f));
      rAvg -= ((vramBank[i] & 0x1f) + (vramBank[i+256] & 0x1f));
    }
    for(int i=48896; i < 49152; ++i)
    {
      vramBank[i] = ARGB16(1,rAvg >> 2,gAvg >> 2,bAvg >> 2);
      bAvg += (((vramBank[i+1] & 0x7c00) + (vramBank[i-255] & 0x7c00)) >> 10);
      bAvg -= (((vramBank[i] & 0x7c00) + (vramBank[i-256] & 0x7c00)) >> 10);
      gAvg += (((vramBank[i+1] & 0x3e0) + (vramBank[i-255] & 0x3e0)) >> 5);
      gAvg -= (((vramBank[i] & 0x3e0) + (vramBank[i-256] & 0x3e0)) >> 5);
      rAvg += ((vramBank[i+1] & 0x1f) + (vramBank[i-255] & 0x1f));
      rAvg -= ((vramBank[i] & 0x1f) + (vramBank[i-256] & 0x1f));
    }
    */
    vramBank[0] = 0x8000 |(0x1f & (((0x1f & vramBank[0]) + (0x1f & vramBank[1]) + (0x1f & vramBank[256]) + (0x1f & vramBank[257])) >> 2)) |
    (0x3e0 & (((0x3e0 & vramBank[9]) + (0x3e0 & vramBank[1]) + (0x3e0 & vramBank[256]) + (0x3e0 & vramBank[257]))>> 2)) |
    (0x7c00 & (((0x7c00 & vramBank[9]) + (0x7c00 & vramBank[1]) + (0x7c00 & vramBank[256]) + (0x7c00 & vramBank[257]))>> 2));
    for(int i = 1; i < 256; ++i)
    {
      vramBank[i] = 0x8000 |(0x1f & (((0x1f & vramBank[i-1]) + (0x1f & vramBank[i+1]) + (0x1f & vramBank[i]) + (0x1f & vramBank[i+256])) >> 2)) |
      (0x3e0 & (((0x3e0 & vramBank[i-1]) + (0x3e0 & vramBank[i+1]) + (0x3e0 & vramBank[i]) + (0x3e0 & vramBank[i+256]))>> 2)) |
      (0x7c00 & (((0x7c00 & vramBank[i-1]) + (0x7c00 & vramBank[i+1]) + (0x7c00 & vramBank[i]) + (0x7c00 & vramBank[i+256]))>> 2));
    }
    for(int i = 256; i < 48896; ++i)
    {
      vramBank[i] = 0x8000 |(0x1f & (((0x1f & vramBank[i-1]) + (0x1f & vramBank[i+1]) + (0x1f & vramBank[i-256]) + (0x1f & vramBank[i+256])) >> 2)) |
      (0x3e0 & (((0x3e0 & vramBank[i-1]) + (0x3e0 & vramBank[i+1]) + (0x3e0 & vramBank[i-256]) + (0x3e0 & vramBank[i+256]))>> 2)) |
      (0x7c00 & (((0x7c00 & vramBank[i-1]) + (0x7c00 & vramBank[i+1]) + (0x7c00 & vramBank[i-256]) + (0x7c00 & vramBank[i+256]))>> 2));
    }
    for(int i = 48896; i < 49152; ++i)
    {
      vramBank[i] = 0x8000 |(0x1f & (((0x1f & vramBank[i-1]) + (0x1f & vramBank[i+1]) + (0x1f & vramBank[i-256]) + (0x1f & vramBank[i])) >> 2)) |
      (0x3e0 & (((0x3e0 & vramBank[i-1]) + (0x3e0 & vramBank[i+1]) + (0x3e0 & vramBank[i-256]) + (0x3e0 & vramBank[i]))>> 2)) |
      (0x7c00 & (((0x7c00 & vramBank[i-1]) + (0x7c00 & vramBank[i+1]) + (0x7c00 & vramBank[i-256]) + (0x7c00 & vramBank[i]))>> 2));
    }
  }
}

void LevelSet::gameOver(Level* ended)
{
  vramSetBankD(VRAM_D_LCD);

  captureScreen(3);
  
  delete ended;
  
  videoSetMode(MODE_FB3);
  
  blurScreen(5,3,false);
  
  swiWaitForVBlank();
  Settings::get().getEnergyBar()->hide();
  Settings::get().getOverloadBar()->hide();
  vramSetBankD(VRAM_D_MAIN_BG);
  videoSetMode(MODE_3_2D | DISPLAY_SPR_1D | DISPLAY_SPR_1D_SIZE_64);
  oamEnable(&oamMain);
  bgInit(BEAMBG_IDX, BgType_Bmp16, BgSize_B16_256x256, 24, 0);
  BgHandles& handles = Settings::get().getBgHandles();
  int curPalIdx = Settings::get().getPalIdx();
  bgSetScroll(handles.beamBg,0,0);

  bgHide(handles.gameBg);
  bgHide(handles.mirrorsBg);
  bgHide(handles.decorBg);
  bgUpdate();
  
  int * velocity = new int[8];
  int * position = new int[8];
  Sprite* sprites[] = {
    new Sprite(true, coord(56,-16), 0, curPalIdx, SpriteSize_16x16, SpriteColorFormat_16Color, false, false, false, false , 1, true),
    new Sprite(true, coord(72,-16), 0, curPalIdx, SpriteSize_16x16, SpriteColorFormat_16Color, false, false, false, false , 1, true),
    new Sprite(true, coord(88,-16), 0, curPalIdx, SpriteSize_16x16, SpriteColorFormat_16Color, false, false, false, false , 1, true),
    new Sprite(true, coord(104,-16), 0, curPalIdx, SpriteSize_16x16, SpriteColorFormat_16Color, false, false, false, false , 1, true),
    new Sprite(true, coord(136,-16), 0, curPalIdx, SpriteSize_16x16, SpriteColorFormat_16Color, false, false, false, false , 1, true),
    new Sprite(true, coord(152,-16), 0, curPalIdx, SpriteSize_16x16, SpriteColorFormat_16Color, false, false, false, false , 1, true),
    new Sprite(true, coord(168,-16), 0, curPalIdx, SpriteSize_16x16, SpriteColorFormat_16Color, false, false, false, false , 1, false),
    new Sprite(true, coord(184,-16), 0, curPalIdx, SpriteSize_16x16, SpriteColorFormat_16Color, false, false, false, false , 1, true)
  };
  
  //copy Game Over gfx
  dmaCopy(&gameoverTiles, sprites[0]->getGfxPtrs()[0], 128);
  dmaCopy(128+((char*)&gameoverTiles), sprites[1]->getGfxPtrs()[0], 128);
  dmaCopy(256+((char*)&gameoverTiles), sprites[2]->getGfxPtrs()[0], 128);
  dmaCopy(384+((char*)&gameoverTiles), sprites[3]->getGfxPtrs()[0], 128);
  dmaCopy(512+((char*)&gameoverTiles), sprites[4]->getGfxPtrs()[0], 128);
  dmaCopy(640+((char*)&gameoverTiles), sprites[5]->getGfxPtrs()[0], 128);
  sprites[6]->setGfxPtrs(sprites[3]->getGfxPtrs());
  dmaCopy(768+((char*)&gameoverTiles), sprites[7]->getGfxPtrs()[0], 128);
  
  oamUpdate(&oamMain);
  
  for(int i=0; i<8; ++i)
  {
    velocity[i] = 0;
    position[i] = -16;
  }
  
  velocity[0] = 1;
  
  bool stopped = false;
  while(!stopped)
  {
    for(int i=0; i<8; ++i)
    {
      position[i] = position[i] + velocity[i];
      if(position[i] > -16 && position[i] < 70)
      {
        velocity[i] += 1;
      }
      if(position[i] >= 70 && velocity[i] > 0)
      {
        velocity[i] = -(velocity[i]) + 3 + (rand() & 3);
        position[i] = 70;
        if(velocity[i] >= 0)
        {
          //come to rest
          velocity[i] = 0;
          position[i] = 70;
        }
        if(i < 7 && position[i+1] == -16)
        {
          velocity[i+1] = 1;
          position[i+1] = -15;
        }
      }
      sprites[i]->setYPos(position[i]);
    }
    
    swiWaitForVBlank();
    oamUpdate(&oamMain);
    stopped = true;
    for(int i=0; i<8; ++i)
    {
      stopped = stopped && velocity[i] == 0 && position[i] == 70;
      if(!stopped) break;
    }
  }
  
  delete [] velocity;
  delete [] position;
  
  timerStart(0, ClockDivider_1024, 1, timerHandler);
  for(int i=0; i<2; ++i)
  {
    swiIntrWait(1, IRQ_TIMER0);
  }
  //disable timer
  TIMER_CR(0) = 0;
  
  vramSetBankB(VRAM_B_LCD);

  captureScreen(1);
  
  videoSetMode(MODE_FB1);
  
  blurScreen(16,1, true);
  
  videoSetMode(MODE_3_2D | DISPLAY_SPR_1D | DISPLAY_SPR_1D_SIZE_64);
  
  //clear bank B
  DMA_FILL(3) = (vuint32)0;
  DMA_SRC(3) = (uint32)&DMA_FILL(3);
  DMA_DEST(3) = (uint32)VRAM_B;
  DMA_CR(3) = DMA_SRC_FIX | DMA_COPY_WORDS | ((SCREEN_WIDTH*192)>>1);
  
  //hide gameover sprites
  for(int i=0; i < 8; ++i)
  {
    delete sprites[i];
  }
  oamEnable(&oamMain);
  
  bgInit(BEAMBG_IDX, BgType_Bmp16, BgSize_B16_256x256, 3, 0);  
  bgSetScroll(handles.beamBg,1-XOFS,1-YOFS);
  bgShow(handles.gameBg);
  bgShow(handles.mirrorsBg);
  bgShow(handles.decorBg);
  disableFadeSub();
  bgUpdate();
  oamUpdate(&oamMain);
}

LevelSet::LevelSet(char* dir) : currentScore(0), currentLives(3), currentLevel(0), dirLen(strlen(dir) + 1)
{
  memset(filename, 0, MAXPATHLEN);
  strncpy(filename, dir, MAXPATHLEN);
  filename[dirLen-1] = '/';
}

void LevelSet::playLevelSet()
{
  strcpy(filename+dirLen, "filelist.txt");
  FILE *flist = fopen(filename,"r");
  if(flist != NULL)
  {
    char line[128];
    
    while(fgets(line, sizeof line, flist) != NULL)
    {
      if (line[strlen(line) - 1] == '\n') {
        line[strlen(line) - 1] = '\0';
      }
      strcpy(filename+dirLen, line);
      currentLevel++;
      while(!playLevel(filename))
      {
        currentLives--;
        iprintf("lives:%d\n",currentLives);
        if(currentLives < 0)
        {
          fclose(flist);
          return;
        }
      }
    }
    swiWaitForVBlank();
    Settings::get().getEnergyBar()->hide();
    Settings::get().getOverloadBar()->hide();
    oamUpdate(&oamMain);
    fclose(flist);
  }
  else
  {
    iprintf("Can't open %s\n",filename);
  }
  return;
}

bool LevelSet::playLevel(char* levelfile)
{
  BgHandles& handles = Settings::get().getBgHandles();
  Level* tempLevel = new Level(levelfile, handles.gameBg, handles.mirrorsBg, handles.beamBg, Settings::get().getLevelParams());
  disableFade();
  
  snprintf(levelStr, 11, "LEVEL: %03d",currentLevel);
  tempLevel->printSub(11, 12, handles.gamedisplayBg, 0, levelStr);
  snprintf(livesStr, 9, "LIVES: %d",currentLives);
  tempLevel->printSub(12, 17, handles.gamedisplayBg, 2, livesStr);
  int newScore = tempLevel->playLevel(currentScore);

  if(newScore >= 0)
  {
    currentScore = newScore;
    fadeToBlack();
    delete tempLevel;
    return true;
  }
  else if(newScore == -1)
  {
    if(currentLives > 0)
    {
      fadeToBlack();
      delete tempLevel;
    }
    else
    {
      gameOver(tempLevel);
      bgHide(handles.decorBg);
      bgUpdate();
    }
    return false;
  }
  else// if(newScore == -2)
  {
    currentLives = 0;
    fadeToBlack();
    delete tempLevel;
    bgHide(handles.decorBg);
    bgUpdate();
    Settings::get().getEnergyBar()->hide();
    Settings::get().getOverloadBar()->hide();
    oamUpdate(&oamMain);
    return false;
  }
}
