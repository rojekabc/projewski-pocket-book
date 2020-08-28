#ifndef SOKOBAN_BITMAPS_H
#define SOKOBAN_BITMAPS_H

#include <inkview.h>

void bitmaps_setTileSize(int size);
const ibitmap *bitmaps_getTile(int enumBoardElement, int isRight);

#endif //SOKOBAN_BITMAPS_H
