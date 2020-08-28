#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <dlfcn.h>
#include "inkview.h"
#include "config.h"
#include "collection.h"
#include "boardelement.h"
#include "bitmaps.h"
#include "tool/debug.h"
#include "tool/mystr.h"
#include "tool/istream.h"
#include "tool/fileistream.h"

#define historyLimit 1000
#define boardMaxSize 25 //Hard to tell - currently this is 25 = (800 - 50)/30.

char *level;

#ifdef PLATFORM_FW5
#	define KEY_OK IV_KEY_OK
#	define KEY_BACK IV_KEY_BACK
#	define KEY_PLUS IV_KEY_PLUS
#	define KEY_MINUS IV_KEY_MINUS
#	define KEY_UP IV_KEY_UP
#	define KEY_DOWN IV_KEY_DOWN
#	define KEY_LEFT IV_KEY_LEFT
#	define KEY_RIGHT IV_KEY_RIGHT
#	define KEY_MENU IV_KEY_MENU
#endif

void PrepareBoard(const char* levelFilename);

void DrawBoard();

void PickLevelSet();

void boardLoadLevel() {
    PrepareBoard(config_getLevelFileName());
    DrawBoard();
}

static imenu mainMenu[] = {
        {ITEM_HEADER, 0,   "Menu",                  NULL},
        {ITEM_ACTIVE, 101, "Undo",                  NULL},
        {ITEM_ACTIVE, 102, "Reset level",           NULL},
        {ITEM_ACTIVE, 103, "Next Level",            NULL},
        {ITEM_ACTIVE, 104, "Previous Level",        NULL},
        {ITEM_ACTIVE, 105, "Open Level Collection", NULL},
        {ITEM_ACTIVE, 106, "Help",                  NULL},
        {ITEM_ACTIVE, 107, "About",                 NULL},
        {ITEM_ACTIVE, 108, "Exit",                  NULL},
        {0,           0, NULL,                      NULL}
};

imenu levelMenu[11];

int tileSize;
int playerX, playerY;
int boxCount;
int boxesOnPlace;
int lastMoveLeft = 1;

///****This bunch of items creates something like incapsulated two-dimensional array - that's just because I've forget, and unable to find
//an example, how to manage dynamic two-dimensional array in C
int boardHeight;
int boardWidth;
char board[boardMaxSize * boardMaxSize/*max (currently) supported linear level size is size of screen*/];


int getCoordinateForXY(int x, int y) {
    return x + y * boardWidth;
}

char getBoard(int x, int y) {
    if (x < 0 || x >= boardWidth || y < 0 || y >= boardHeight) {
        return Wall;
        //if someone is asking about the world around level - there are only bricks. This will help in case of open level
    }
    return board[getCoordinateForXY(x, y)];
}

void setBoard(int x, int y, char value) {
    if (x >= 0 && x < boardWidth && y >= 0 && y < boardHeight) {
        //we're writing data only while inbounds of board - also temporary helps when board is too big
        board[getCoordinateForXY(x, y)] = value;
    }
}

void boardReset() {
    //Set board size to (currently) max available - TODO: think what to do with this
    boardWidth = boardMaxSize;
    boardHeight = boardMaxSize;
    int i, j;
    for (i = 0; i < boardMaxSize; i++) {
        for (j = 0; j < boardMaxSize; j++) {
            setBoard(i, j, Empty);
        }
    }
}

void boardCompress(int newWidth, int newHeight) {
    //old width was boardMaxSize - but in fact items are present only in first newWidth items of each row, and in first newHeight rows. so...
    int i, j;
    for (j = 0; j < newHeight; j++) {
        for (i = 0; i < newWidth; i++) {
            //order of cycles here has meaning - we're moving row by row, emptying space for next ones, not column by column - as they are stored in
            board[i + j * newWidth] = board[i + j * boardMaxSize];
        }
    }

    int width50 = ScreenWidth() / 50;
    int height50 = (ScreenHeight() - 50) / 50;
    int width30 = ScreenWidth() / 30;
    int height30 = (ScreenHeight() - 50) / 30;

    if (newWidth <= width50 && newHeight <= height50) {
        boardWidth = newWidth;
        boardHeight = newHeight;
        tileSize = 50;

    } else {
        //prevention of too big level
        boardWidth = newWidth > width30 ? width30 : newWidth;
        boardHeight = newHeight > height30 ? height30 : newHeight;
        tileSize = 30;
    }
    bitmaps_setTileSize(tileSize);
}
///****end of board

///****another pseudo-class - stack of moves, limited to 1000 moves, then starts to overwrite
int moves[3 * historyLimit];
int movesPointer = -1;
int undoTill = -1;

void resetMovesStack() {
    movesPointer = -1;
    undoTill = -1;
}

void pushMove(int dx, int dy, int pull) {
    movesPointer++;
    if (movesPointer >= historyLimit && movesPointer - historyLimit > undoTill) {
        undoTill = movesPointer - historyLimit;
    }
    moves[movesPointer % historyLimit * 3] = dx;
    moves[movesPointer % historyLimit * 3 + 1] = dy;
    moves[movesPointer % historyLimit * 3 + 2] = pull;
}

//if stack is not empty, returns 1
int popMove(int *dx, int *dy, int *pull) {
    if (movesPointer > undoTill) {
        *dx = moves[movesPointer % historyLimit * 3];
        *dy = moves[movesPointer % historyLimit * 3 + 1];
        *pull = moves[movesPointer % historyLimit * 3 + 2];
        movesPointer--;
        return 1;
    } else if (undoTill != -1) {
        Message(ICON_WARNING, "Sorry",
                "No more undo available", 10000);
    }
    return 0;
}
///****end of movement stack

///****nextLine This part of code will try to reproduce functionality of strtok(char*, "\r\n")
///again have no idea how to create new char* - so using global variable for response
int levelIndex = 0;

/*
char currentLine[100];

int getNextLine() {
    if (level[levelIndex] == 0) {
        return 0;
    }
    int lineBegin = levelIndex;
    //seek for end
    while (level[levelIndex] != '\r' && level[levelIndex] != '\n'
           && level[levelIndex] != 0) {
        levelIndex++;
    }
    int lineEnd = levelIndex;
    //this to remove trailing blanks
    while (level[lineEnd - 1] == Empty) {
        lineEnd--;
    }
    //seek for first character of next line
    while (level[levelIndex] == '\r' || level[levelIndex] == '\n') {
        levelIndex++;
    }
    int i;
    for (i = 0; i < lineEnd - lineBegin; i++) {
        currentLine[i] = level[lineBegin + i];
    }
    currentLine[lineEnd - lineBegin] = 0;
    return 1;
}

///****end of nextLine
*/
int baseX;
int baseY;

//moves player to the position (+dx, +dy) if possible
//returns 1 if succeeded, 0 else
int movePlayer(int dx, int dy) {
    char destinationElement = getBoard(playerX + dx, playerY + dy);
    if (destinationElement != Empty && destinationElement != Spot) {
        return 0;
    }

    if (destinationElement == Empty) {
        setBoard(playerX + dx, playerY + dy, Player);
    } else {
        setBoard(playerX + dx, playerY + dy, PlayerOnTheSpot);
    }

    char sourceElement = getBoard(playerX, playerY);
    if (sourceElement == Player) {
        setBoard(playerX, playerY, Empty);
    } else {
        setBoard(playerX, playerY, Spot);
    }
    return 1;
}

//actually, if there is box and it is possible, moves box from position (x, y) to (x + dx, y + dy)
int moveBox(int x, int y, int dx, int dy) {
    char sourceElement = getBoard(x, y);
    char destinationElement = getBoard(x + dx, y + dy);
    if ((sourceElement == Box || sourceElement == BoxOnTheSpot) &&
        (destinationElement == Empty || destinationElement == Spot)) {
        if (destinationElement == Empty) {
            setBoard(x + dx, y + dy, Box);
        } else {
            setBoard(x + dx, y + dy, BoxOnTheSpot);
            boxesOnPlace++;
        }
        if (sourceElement == Box) {
            setBoard(x, y, Empty);
        } else {
            setBoard(x, y, Spot);
            boxesOnPlace--;
        }
        return 1;
    }
    return 0;
}

void PrepareBoard(const char* levelFilename) {
    boardReset();
    resetMovesStack();
    int currentLineIndex = 0;
    levelIndex = 0;
    size_t currentLevelWidth = 0;
    int currentLevelHeight = 0;
    size_t i;
    boxCount = 0;
    boxesOnPlace = 0;

    GOC_IStream* iStream = goc_fileIStreamOpen(levelFilename);
    char* line = NULL;

    while ((string_free(line), line = goc_isReadLine(iStream))) {
        if (goc_stringStartsWith(line, "Author: ") || goc_stringStartsWith(line, "Title: ")) {
            continue;
        }
        currentLevelHeight++;
        if (currentLevelWidth < strlen(line)) {
            currentLevelWidth = strlen(line);
        }
        for (i = 0; i < strlen(line); i++) {
            setBoard(i, currentLineIndex, line[i]);
            if (line[i] == Player || line[i] == PlayerOnTheSpot) {
                playerX = i;
                playerY = currentLineIndex;
            }
            if (line[i] == Box) {
                boxCount++;
            }
            if (line[i] == BoxOnTheSpot) {
                boxCount++;
                boxesOnPlace++;
            }
        }
        currentLineIndex++;
    }
    goc_isClose(iStream);
    boardCompress(currentLevelWidth, currentLevelHeight);

    baseX = (ScreenWidth() - boardWidth * tileSize) / 2;
    baseY = (ScreenHeight() - 50 - boardHeight * tileSize) / 2;
}

void DrawCell(int i, int j) {
    //NOTE: failed usage of ibitmap *image = &name; and then DrawBitmap(,, image); - find out why?
    int x = baseX + i * tileSize;
    int y = baseY + j * tileSize;
    DrawBitmap(x, y, bitmaps_getTile(getBoard(i, j), !lastMoveLeft));
}

void UpdateRegion(int x, int y, int dx, int dy) {
    int minx = dx > 0 ? x : x + dx;
    int width = 1 + (dx < 0 ? -dx : dx);
    int miny = dy > 0 ? y : y + dy;
    int height = 1 + (dy < 0 ? -dy : dy);

    PartialUpdateBW(baseX + minx * tileSize, baseY + miny * tileSize, width * tileSize, height
                                                                                        * tileSize);
}

void DrawBoard() {
    ClearScreen();
    int i, j;
    for (i = 0; i < boardWidth; i++) {
        for (j = 0; j < boardHeight; j++) {
            DrawCell(i, j);
        }
    }
    /*
    SetFont(font, WHITE);

    FillArea(0, ScreenHeight() - 50, ScreenWidth(), 50, DGRAY);
    char buf[16];
    sprintf(buf, "Level %i/%i", levelNo + 1, levels + 1);
    DrawString(5, ScreenHeight() - 45, buf);
    DrawString(5, ScreenHeight() - 20, "Move: Up/Down/Left/Right. Undo: OK. Menu: Hold OK. Level navigation: +/-");
    */
    FullUpdate();
}

void Move(int dx, int dy) {
    //if there is box in front of player - it will be pushed one square further if possible.
    //Then, if there free space in front of player - it definitely will, if there was box and it was moved - he'll move himself
    //sum of this actions will inform us how much squares to update
    int refresh = 0;
    if (dx > 0) {
        lastMoveLeft = 1;
        refresh = 1;
    } else if (dx < 0) {
        lastMoveLeft = 0;
        refresh = 1;
    }
    int shift = moveBox(playerX + dx, playerY + dy, dx, dy) + movePlayer(dx, dy);
    if (shift != 0) {
        DrawCell(playerX, playerY);
        DrawCell(playerX + dx, playerY + dy);
        if (shift == 2) {
            DrawCell(playerX + 2 * dx, playerY + 2 * dy);
        }
        //FullUpdate();
        UpdateRegion(playerX, playerY, shift * dx, shift * dy);
        playerX = playerX + dx;
        playerY = playerY + dy;
        //last value allows to distinguish, was box pushed on this stage.
        pushMove(dx, dy, shift - 1);
    } else if (refresh != 0) {
        DrawCell(playerX, playerY);
        UpdateRegion(playerX, playerY, 0, 0);
    }

    // Check if the player wins current level
    if (boxesOnPlace == boxCount) {
        int levelNo = config_getLevel();
        if (levelNo < collection_current()->levels) {
            Message(ICON_INFORMATION, "Congratulations!",
                    "You completed the level, press any key to continue", 10000);
            config_setLevel(collection_current()->folder, levelNo+1);
            boardLoadLevel();
        } else {
            Message(ICON_INFORMATION, "Congratulations!",
                    "You completed all the levels", 10000);
            PickLevelSet();
        }
    }

}

/*
void loadLevel(char* levelFilename) {
    debug("Loading level file %s\n", levelFilename);
    FILE *levelFile = fopen(levelFilename, "rb");
    if (levelFile == NULL) {
        return;
    }

    fseek(levelFile, 0, SEEK_END);
    int fileSize = ftell(levelFile);
    rewind(levelFile);
    free(level);
    level = (char *) malloc(sizeof(char) * fileSize);
    fread(level, 1, fileSize, levelFile);
    fclose(levelFile);
}
 */

int collectionPickerStart = 0;
int collectionMenuPosition = 0;
#define COLLECTION_MENU_MAX 7
#define COLLECTION_NEXT_INDEX -2
#define COLLECTION_PREV_INDEX -3
#define COLLECTION_NONE_INDEX -1


void loadLevelsHandler(int index) {
    switch (index) {
        case COLLECTION_NONE_INDEX:
            break;
        case COLLECTION_NEXT_INDEX:
            collectionPickerStart += COLLECTION_MENU_MAX;
            debug("next %d\n", collectionPickerStart);
            PickLevelSet();
            break;
        case COLLECTION_PREV_INDEX:
            collectionPickerStart -= COLLECTION_MENU_MAX;
            debug("prev %d\n", collectionPickerStart);
            PickLevelSet();
            break;
        default: {
            Collection *collection = goc_arrayGet(collections_retrieve(), index);
            collection_select(collection->folder);
            boardLoadLevel();
            break;
        }
    }

}

void collectionMenu_setItem(short menuType, short menuIndex, const char *menuText) {
    levelMenu[collectionMenuPosition].type = menuType;
    levelMenu[collectionMenuPosition].index = menuIndex;
    string_set(levelMenu[collectionMenuPosition].text, (char *) menuText);
    collectionMenuPosition++;
}

void PickLevelSet() {
    collectionMenuPosition = 0;
    GOC_Array *collections = collections_retrieve();
    debug("Collection Picker %d\n", collectionPickerStart);
    collectionMenu_setItem(ITEM_HEADER, COLLECTION_NONE_INDEX, "Choose collection");
    if (collectionPickerStart) {
        collectionMenu_setItem(ITEM_ACTIVE, COLLECTION_PREV_INDEX, "< Previous");
    }

    int pos = 0;
    for (; (pos < COLLECTION_MENU_MAX) && (collectionPickerStart + pos < collections->nElement); pos++) {
        Collection* collection = goc_arrayGet(collections, collectionPickerStart+pos);
        char *title = collection->title;
        debug("Setup title as [%d] %s\n", collectionPickerStart + pos, title);
        collectionMenu_setItem(ITEM_ACTIVE, (short) (collectionPickerStart + pos), title);
    }
    if (pos == COLLECTION_MENU_MAX) {
        collectionMenu_setItem(ITEM_ACTIVE, COLLECTION_NEXT_INDEX, "Next >");
    }
    collectionMenu_setItem(0, COLLECTION_NONE_INDEX, NULL);

    OpenMenu(levelMenu, 0, 30, 30, loadLevelsHandler);
}

void Undo() {
    int dx, dy, pull;
    while (popMove(&dx, &dy, &pull)) {
        //This is reversed movement of player
        movePlayer(-dx, -dy);
        if (pull) {
            //and if there was push on previous movement - we're reverting it
            moveBox(playerX + dx, playerY + dy, -dx, -dy);
        }
        playerX = playerX - dx;
        playerY = playerY - dy;

        DrawCell(playerX, playerY);
        DrawCell(playerX + dx, playerY + dy);
        if (pull == 1) {
            DrawCell(playerX + 2 * dx, playerY + 2 * dy);
        }
        UpdateRegion(playerX, playerY, (pull + 1) * dx, (pull + 1) * dy);
        if (pull == 1) {
            return;
        }
    }
}

int selectedIndex = 0;

void mainMenuHandler(int index) {
    int levelNo = config_getLevel();
    Collection * collection = collection_current();

    selectedIndex = index;
    switch (index) {
        case 101:
            Undo();
            break;
        case 102:
            boardLoadLevel();
            break;
        case 103:
            if (levelNo < collection->levels) {
                config_setLevel(collection->folder, levelNo+1);
                boardLoadLevel();
            }
            break;
        case 104:
            if (levelNo > 0) {
                config_setLevel(collection->folder, levelNo-1);
                boardLoadLevel();
            }
            break;
        case 105:
            PickLevelSet();
            break;
        case 106:
            Message(0, "Help",
                    "Sokoban is a classic puzzle game invented in Japan. The original game of SOKOBAN was written by Hiroyuki Imabayashi.\n"
                    "Today Sokoban is one of the most popular thought and logic games.\n"
                    "The name comes from Japan and means \"warehouse keeper\".\n"
                    "As simple as that name is, so is the idea of this game: A level represents a store room, where boxes appear to be randomly placed. You help the warehouse keeper to push the boxes around the maze of the room so that, at the end, all boxes are on marked fields. The only restrictions are that the warehouse keeper can only push a box, never pull, and that just one box may be pushed at a time.\n"
                    "Each level has a different structure, which requires a different solution - often with a different strategy as well. The magic of the game is that boxes, which are out of the way on one move, might be in the way on the next move.\n"
                    "The simplicity of the rules, in combination with the many levels ranging from easy to extremely difficult, have made Sokoban become a real classic.\n"
                    "(Sokoban Wiki: http://www.sokobano.de/)\n\n"
                    "You can control warehause keeper by using direction buttons. If you get stuck - Ok button will help you to undo last action, or menu - to reset the level entirely.\n"
                    "You can also use +/- buttons to navigate across the levels, or use menu to open new level sets\n"
                    "If you want to extend your game with new levels you can download them in internet and put in the \\system\\share\\sokoban folder in your Pocketbook memory.",
                    60000);
            break;
        case 107:
            Message(0, "About",
                    "Socoban for Pocketbook\nDeveloped by Andriy Kvasnytsya, 2009\nDefault level set is Minicosmos set by Aymeric du Peloux",
                    10000);
            break;
        case 108:
            CloseApp();
            break;
    }
}

void ShowMenu() {
    OpenMenu(mainMenu, selectedIndex, 20, 20, mainMenuHandler);
}

void KeyPressed(int key) {
    int levelNo = config_getLevel();
    Collection* collection = collection_current();

    switch (key) {
        case KEY_BACK:
            CloseApp();
            return;
        case KEY_PLUS:
            if (levelNo < collection->levels) {
                config_setLevel(collection->folder, levelNo+1);
                boardLoadLevel();
            }
            return;
        case KEY_MINUS:
            if (levelNo > 0) {
                config_setLevel(collection->folder, levelNo-1);
                boardLoadLevel();
            }
            return;
        case KEY_UP:
            Move(0, -1);
            break;
        case KEY_DOWN:
            Move(0, 1);
            break;
        case KEY_LEFT:
            Move(-1, 0);
            break;
        case KEY_RIGHT:
            Move(1, 0);
            break;
        case KEY_OK:
            Undo();
            break;
        case KEY_MENU:
            ShowMenu();
            break;
    }
}

int main_handler(int type, int par1, int par2) {
    if (type == EVT_INIT) {
        OpenFont("LiberationSans", 16, 0);
        PrepareBoard(config_getLevelFileName());
    } else if (type == EVT_SHOW) {
        DrawBoard();
    } else if (type == EVT_KEYPRESS && par1 != KEY_OK) {
        KeyPressed(par1);
    } else if (type == EVT_KEYREPEAT && par1 == KEY_OK) {
        KeyPressed(KEY_MENU);
    } else if (type == EVT_KEYRELEASE && par1 == KEY_OK && par2 == 0) {
        KeyPressed(KEY_OK);
    } else if (type == EVT_POINTERUP) {
        int w = ScreenWidth();
        int h = ScreenHeight();
        // y - y1 / y1 - y2 = x - x1 / x1 - x2
        // (y - y1) * (x1 - x2) = (x - x1) * (y1 - y2)
        // Prosta nr 1
        // (y - h) * w = (x - w) * h
        // wy - wh = hx - wh
        // wy - hx = 0
        //
        // Prosta nr 2
        // -wy + wh = hx
        // -wy - hx + wh = 0
        // wy + hx - wh = 0
        int wpar2 = w * par2;
        int hpar1 = h * par1;

        int one = wpar2 - hpar1;
        int two = wpar2 + hpar1 - w * h;

        // TODO: Get Orientation
        if (one > 0) {
            if (two > 0) { // right side screen
                // Move(1, 0);
                Move(0, 1);
            } else { // up side screen
                // Move(0, -1);
                Move(-1, 0);
            }
        } else {
            if (two > 0) { // down side screen
                // Move(0, 1);
                Move(1, 0);
            } else { // left side screen
                // Move(-1, 0);
                Move(0, -1);
            }
        }
    }
    return 0;
}

int main() {
#ifdef PLATFORM_FW5
    OpenScreen();
    // This operation needs OpenScreen call before, or SEGV
    // And it disables
    SetShowPanelReader(0);
#endif
    memset(levelMenu, 0, sizeof(imenu) * 11);

    collection_select(config_getCollectionName());
    if (collection_current() == NULL) {
        Message(ICON_ERROR, "Error", "Cannot load levels collection", 5000);
        return 1;
    }
    InkViewMain(main_handler);
    return 0;
}
