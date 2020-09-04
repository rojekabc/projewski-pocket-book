#include "stagestart.h"
#include <inkview.h>
#include "button.h"
#include "config.h"
#include "stagemain.h"
#include "tool/debug.h"
#include "tool/mystr.h"

#define TITLE_FONT_SIZE 120
ifont *titleFont = NULL;
InkButton *buttonSlot1 = NULL;
InkButton *buttonSlot2 = NULL;
InkButton *buttonSlot3 = NULL;
InkButton *buttonSlotAnonymous = NULL;
#define TITLE_Y_POSITION 200

enum StageModes {
    PageMode,
    KeyboardMode
};
int mode = PageMode;

void stageStart_show();

void stageStart_pointerHit(int x, int y);

void stageStart_init();

int stageStart_handle(int type, int par1, int par2) {
    if (type == EVT_SHOW) {
        debug("stageStart_handle EVT_SHOW %d %d\n", par1, par2);
        if (mode == PageMode) {
            stageStart_show();
        }
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
    const char *slotUser = config_getSlotUsername(0);
    buttonSlot1 = inkButton_alloc(xPosition, yPosition, slotUser ? slotUser : "Slot 1");
    yPosition += buttonSlot1->height + BUTTON_DISTANCE;
    slotUser = config_getSlotUsername(1);
    buttonSlot2 = inkButton_alloc(xPosition, yPosition, slotUser ? slotUser : "Slot 2");
    yPosition += buttonSlot2->height + BUTTON_DISTANCE;
    slotUser = config_getSlotUsername(2);
    buttonSlot3 = inkButton_alloc(xPosition, yPosition, slotUser ? slotUser : "Slot 3");
    yPosition += buttonSlot3->height + BUTTON_DISTANCE;
    buttonSlotAnonymous = inkButton_alloc(xPosition, yPosition, "Anonymous");

    buttonSlot1->width = width;
    buttonSlot2->width = width;
    buttonSlot3->width = width;
    buttonSlotAnonymous->width = width;

}

void enter_username();

void enter_password();

void keyboard_handler_password(char *text) {
    debug("-> keyboard_handler_password %s\n", text);
    while (1) {
        mode = PageMode;
        const char *password = config_getPassword();
        if (password) {
            if (goc_stringEquals(text, password)) {
                select_stage(stage_game);
            } // else wrong password
        } else {
            config_setPassword(text);
            select_stage(stage_game);
        }
        break;
    }
    string_free(text);
    debug("<- keyboard_handler_password\n");
}

#define PASSWORD_LENGTH 4

void enter_password() {
    debug("-> enter_password\n");
    char *keyboardUsername = malloc(PASSWORD_LENGTH + 1);
    memset(keyboardUsername, 0, PASSWORD_LENGTH + 1);
    OpenKeyboard("User PIN (4 numbers)", keyboardUsername, PASSWORD_LENGTH - 1,
                 KBD_NUMERIC | KBD_PASSWORD | KBD_NOUPDATE_AFTER_CLOSE, keyboard_handler_password);
    debug("<- enter_password\n");
}

void keyboard_handler_username(char *text) {
    debug("-> keyboard_handler_username\n");
    while (1) {
        if (text == NULL || strlen(text) == 0) {
            mode = PageMode;
            break;
        }
        config_setUsername(text);
        enter_password();
        break;
    }
    string_free(text);
    debug("<- keyboard_handler_username\n");
}

#define USERNAME_LENGTH 16

void enter_username() {
    debug("-> enter_username\n");
    mode = KeyboardMode;
    char *keyboardUsername = malloc(USERNAME_LENGTH + 1);
    memset(keyboardUsername, 0, USERNAME_LENGTH + 1);
    OpenKeyboard("User name", keyboardUsername, USERNAME_LENGTH - 1,
                 KBD_NORMAL | KBD_FIRSTUPPER | KBD_NOUPDATE_AFTER_CLOSE,
                 keyboard_handler_username);
    debug("<- enter_username\n");
}

void stageStart_pointerHit(int x, int y) {
    debug("-> stageStart_pointerHit\n");
    if (inkObject_is_inside(buttonSlot1, x, y)) {
        config_slot_select(0);
    } else if (inkObject_is_inside(buttonSlot2, x, y)) {
        config_slot_select(1);
    } else if (inkObject_is_inside(buttonSlot3, x, y)) {
        config_slot_select(2);
    } else if (inkObject_is_inside(buttonSlotAnonymous, x, y)) {
        config_slot_select(3);
    }
    const char *username = config_getUsername();
    if (username == NULL) {
        enter_username();
    } else {
        const char *password = config_getPassword();
        if (password && strlen(password)) {
            enter_password();
        }
    }
    debug("<- stageStart_pointerHit\n");
}