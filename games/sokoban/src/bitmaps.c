#include "bitmaps.h"
#include "boardelement.h"

extern const ibitmap box30, boxOnTheSpot30, empty30, player30, playerRight30, playerOnTheSpot30, playerOnTheSpotRight30, spot30, wall30;
extern const ibitmap box50, boxOnTheSpot50, empty50, player50, playerRight50, playerOnTheSpot50, playerOnTheSpotRight50, spot50, wall50;

const ibitmap *bitmapBox;
const ibitmap *bitmapBoxOnTheSpot;
const ibitmap *bitmapEmpty;
const ibitmap *bitmapPlayer;
const ibitmap *bitmapPlayerRight;
const ibitmap *bitmapPlayerOnTheSpot;
const ibitmap *bitmapPlayerOnTheSpotRight;
const ibitmap *bitmapSpot;
const ibitmap *bitmapWall;

const ibitmap *bitmaps_getTile(int enumBoardElement, int isRight) {
    switch (enumBoardElement) {
        case Box:
            return bitmapBox;
        case BoxOnTheSpot:
            return bitmapBoxOnTheSpot;
        case Empty:
            return bitmapEmpty;
        case Player:
            return isRight ? bitmapPlayerRight : bitmapPlayer;
        case PlayerOnTheSpot:
            return isRight ? bitmapPlayerOnTheSpotRight : bitmapPlayerOnTheSpot;
        case Spot:
            return bitmapSpot;
        case Wall:
        default:
            return bitmapWall;
    }
}

void bitmaps_setTileSize(int size) {
    switch (size) {
        case 30:
            bitmapBox = &box30;
            bitmapBoxOnTheSpot = &boxOnTheSpot30;
            bitmapEmpty = &empty30;
            bitmapPlayer = &player30;
            bitmapPlayerRight = &playerRight30;
            bitmapPlayerOnTheSpot = &playerOnTheSpot30;
            bitmapPlayerOnTheSpotRight = &playerOnTheSpotRight30;
            bitmapSpot = &spot30;
            bitmapWall = &wall30;
            break;
        case 50:
            bitmapBox = &box50;
            bitmapBoxOnTheSpot = &boxOnTheSpot50;
            bitmapEmpty = &empty50;
            bitmapPlayer = &player50;
            bitmapPlayerRight = &playerRight50;
            bitmapPlayerOnTheSpot = &playerOnTheSpot50;
            bitmapPlayerOnTheSpotRight = &playerOnTheSpotRight50;
            bitmapSpot = &spot50;
            bitmapWall = &wall50;
            break;
        default:
            break;
    }
}