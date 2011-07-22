
#include <nds.h>

#include "spinmirror.h"
#include "beam.h"

SpinMirror::SpinMirror(int bg, int mirrorBg, int x, int y, unsigned int palIdx, unsigned int tileBase, BeamDirection initAngle) : 
Mirror(bg, mirrorBg, x, y, palIdx, tileBase),
frameCounter(5)
{
  //drawUpdate();
}

void SpinMirror::drawUpdate()
{
  if(frameCounter > ANIMATION_DELAY)
  {
    angle++;
    if(angle & 16) angle = 0; 
    frameCounter = 0;
    u16* mapPtr = (u16*)bgGetMapPtr(bgHandle);
    u16* mirrorMapPtr = (u16*)bgGetMapPtr(mirrorBgHandle);
    //fill in the 4 base tiles on the map
    mirrorMapPtr[pos2idx(xPos*2, yPos*2)] = ((paletteIndex << 12) | mirrorTileBase);
    mirrorMapPtr[pos2idx(xPos*2+1, yPos*2)] = ((paletteIndex << 12) | (mirrorTileBase+1));
    mirrorMapPtr[pos2idx(xPos*2, yPos*2+1)] = ((paletteIndex << 12) | (mirrorTileBase+2));
    mirrorMapPtr[pos2idx(xPos*2+1, yPos*2+1)] = ((paletteIndex << 12) | (mirrorTileBase+3));
    
    //fill in the 4 mirror-position tiles
    int angleTileIdx = 4+(4*angle);
    mapPtr[pos2idx(xPos*2,yPos*2)] = ((paletteIndex << 12) | (mirrorTileBase+angleTileIdx));
    mapPtr[pos2idx(xPos*2+1,yPos*2)] = ((paletteIndex << 12) | (mirrorTileBase+angleTileIdx+1));
    mapPtr[pos2idx(xPos*2,yPos*2+1)] = ((paletteIndex << 12) | (mirrorTileBase+angleTileIdx+2));
    mapPtr[pos2idx(xPos*2+1,yPos*2+1)] = ((paletteIndex << 12) | (mirrorTileBase+angleTileIdx+3));
  }
  else
  {
    frameCounter++;
  }
}
