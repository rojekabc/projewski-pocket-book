//
// Created by piotrek on 9/1/20.
//

#ifndef SOKOBAN_BUTTON_H
#define SOKOBAN_BUTTON_H

#define DEFAULT_BUTTON_FONT_SIZE 64

typedef struct InkButton {
    int x;
    int y;
    int width;
    int height;
    char* title;
} InkButton;

#define IS_BOUNDED(_v_, _a_, _b_) (_v_ >= _a_) && (_v_ <= _b_)
#define inkObject_is_inside(_InkObject_, _x_, _y_) IS_BOUNDED(_x_, _InkObject_->x, _InkObject_->x + _InkObject_->width) && IS_BOUNDED(_y_, _InkObject_->y, _InkObject_->y + _InkObject_->height)

struct InkButton* inkButton_alloc(int x, int y, const char* title);
void inkButton_free(struct InkButton* inkButton);
void inkButton_draw(struct InkButton* inkButton);

#endif //SOKOBAN_BUTTON_H
