//
// Created by piotrek on 9/1/20.
//

#ifndef SOKOBAN_STAGEMAIN_H
#define SOKOBAN_STAGEMAIN_H

enum sokobanStage {
    stage_start,
    stage_game
};

int stage_main_handler(int type, int par1, int par2);
void select_stage(int enum_sokobanStage);

#endif //SOKOBAN_STAGEMAIN_H
