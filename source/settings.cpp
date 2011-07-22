/*
 *  settings.cpp
 *  deflektor-ds
 *
 *  Created by Hugh Cole-Baker on 4/4/09.
 *
 */

#include "settings.h"
#include "bar.h"

Settings* Settings::instance = NULL;

Settings& Settings::get()
{
  if(instance == NULL)
  {
    instance = new Settings();
  }
  return *instance;
}

Settings::Settings() : musicOn(true), sfxOn(true), controlType(true)
{
  
}

LevelParams& Settings:: getLevelParams()
{
  return params;
}

BgHandles& Settings:: getBgHandles()
{
  return handles;
}

SpriteGfx& Settings:: getSpriteGfx()
{
  return spriteGfx;
}

int Settings::getPalIdx()
{
  return palIndex;
}

unsigned int Settings::getMainFontBase()
{
  return mainFontBase;
}

Bar* Settings::getEnergyBar()
{
  return energyBar;
}

Bar* Settings::getOverloadBar()
{
  return overloadBar;
}

bool Settings::getMusicOn()
{
  return musicOn;
}

bool Settings::getSfxOn()
{
  return sfxOn;
}

bool Settings::getControlType()
{
  return controlType;
}

void Settings::setLevelParams(LevelParams params)
{
  this->params = params;
}

void Settings::setBgHandles(BgHandles h)
{
  this->handles = h;
}

void Settings::setSpriteGfx(SpriteGfx gfx)
{
  this->spriteGfx = gfx;
}

void Settings::setPalIdx(unsigned int palIdx)
{
  palIndex = palIdx;
}

void Settings::setMainFontBase(unsigned int fontBase)
{
  mainFontBase = fontBase;
}

void Settings::setEnergyBar(Bar* ebar)
{
  energyBar = ebar;
}

void Settings::setOverloadBar(Bar* obar)
{
  overloadBar = obar;
}

void Settings::setMusicOn(bool value)
{
  musicOn = value;
}

void Settings::setSfxOn(bool value)
{
  sfxOn = value;
}

void Settings::setControlType(bool type)
{
  controlType = type;
}