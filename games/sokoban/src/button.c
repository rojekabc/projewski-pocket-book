#include "button.h"
#include <inkview.h>
#include "tool/malloc.h"
#include "tool/mystr.h"

ifont *buttonFont = NULL;

struct InkButton *inkButton_alloc(int x, int y, const char *title) {
    ALLOC(struct InkButton, result);
    result->x = x;
    result->y = y;
    string_set(result->title, title);
    result->height = DEFAULT_BUTTON_FONT_SIZE + DEFAULT_BUTTON_FONT_SIZE / 5;
    result->width = (int)strlen(title) * DEFAULT_BUTTON_FONT_SIZE / 2 + DEFAULT_BUTTON_FONT_SIZE / 5;
    return result;
}

void inkButton_free(struct InkButton *inkButton) {
    if (inkButton == NULL) {
        return;
    }
    string_free(inkButton->title);
    free(inkButton);
}

void inkButton_draw(struct InkButton *inkButton) {
    if (inkButton == NULL) {
        return;
    }
    if (buttonFont == NULL) {
        buttonFont = OpenFont(DEFAULTFONT, DEFAULT_BUTTON_FONT_SIZE, 0);
    }
    // shadow
    FillArea(inkButton->x + 5, inkButton->y + 5, inkButton->width + 5, inkButton->height + 5, LGRAY);
    // border
    FillArea(inkButton->x, inkButton->y, inkButton->width, inkButton->height, BLACK);
    // internal
    FillArea(inkButton->x + 2, inkButton->y + 2, inkButton->width - 2, inkButton->height - 2, WHITE);
    // Text
    SetFont(buttonFont, BLACK);
    DrawTextRect(inkButton->x + 2, inkButton->y + 2, inkButton->width - 2, inkButton->height - 2, inkButton->title,
                 ALIGN_CENTER | VALIGN_MIDDLE | DOTS | RTLAUTO);
}
