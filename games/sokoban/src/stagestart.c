#include "stagestart.h"
#include <inkview.h>
#include "button.h"
#include "config.h"
#include "stagemain.h"

#define TITLE_FONT_SIZE 120
ifont *titleFont = NULL;
InkButton *buttonSlot1 = NULL;
InkButton *buttonSlot2 = NULL;
InkButton *buttonSlot3 = NULL;
InkButton *buttonSlotAnonymous = NULL;
#define TITLE_Y_POSITION 200

void stageStart_show();

void stageStart_pointerHit(int x, int y);

void stageStart_init();

int stageStart_handle(int type, int par1, int par2) {
    if (type == EVT_SHOW) {
        stageStart_show();
    } else if (type == EVT_POINTERUP) {
        stageStart_pointerHit(par1, par2);
    }
    return 0;
}

void stageStart_show() {
    stageStart_init();
    ClearScreen();
    SetFont(titleFont, BLACK);
    DrawTextRect(0, TITLE_Y_POSITION, ScreenWidth(), TITLE_FONT_SIZE + 20, "S o k o b a n",
                 ALIGN_CENTER | VALIGN_MIDDLE | DOTS | RTLAUTO);
    inkButton_draw(buttonSlot1);
    inkButton_draw(buttonSlot2);
    inkButton_draw(buttonSlot3);
    inkButton_draw(buttonSlotAnonymous);
    FullUpdate();
}

int initialized = 0;
#define BUTTON_DISTANCE 30
void stageStart_init() {
    if (initialized) {
        return;
    } else {
        initialized = 1;
    }

    int xPosition = 200;
    int width = ScreenWidth() - 200 - xPosition;

    titleFont = OpenFont(DEFAULTFONTB, TITLE_FONT_SIZE, 1);

    int yPosition = TITLE_Y_POSITION + TITLE_FONT_SIZE + 40;
    buttonSlot1 = inkButton_alloc(xPosition, yPosition, "Slot 1");
    yPosition += buttonSlot1->height + BUTTON_DISTANCE;
    buttonSlot2 = inkButton_alloc(xPosition, yPosition, "Slot 2");
    yPosition += buttonSlot2->height + BUTTON_DISTANCE;
    buttonSlot3 = inkButton_alloc(xPosition, yPosition, "Slot 3");
    yPosition += buttonSlot3->height + BUTTON_DISTANCE;
    buttonSlotAnonymous = inkButton_alloc(xPosition, yPosition, "Anonymous");

    buttonSlot1->width = width;
    buttonSlot2->width = width;
    buttonSlot3->width = width;
    buttonSlotAnonymous->width = width;

}

void stageStart_pointerHit(int x, int y) {
    if (inkObject_is_inside(buttonSlot1, x, y)) {
        config_slot_select(0);
        select_stage(stage_game);
    } else if (inkObject_is_inside(buttonSlot2, x, y)) {
        config_slot_select(1);
        select_stage(stage_game);
    } else if (inkObject_is_inside(buttonSlot3, x, y)) {
        config_slot_select(2);
        select_stage(stage_game);
    } else if (inkObject_is_inside(buttonSlotAnonymous, x, y)) {
        config_slot_select(3);
        select_stage(stage_game);
    }
}