#include "stagemain.h"
#include <inkview.h>
#include "stagegame.h"
#include "stagestart.h"

int (*current_handler)(int type, int par1, int par2);

void select_stage(int enum_sokobanStage);

int stage_main_handler(int type, int par1, int par2) {
    if (type == EVT_INIT) {
        current_handler = stageStart_handle;
    }
    if (current_handler) {
        return current_handler(type, par1, par2);
    }
}

void select_stage(int enum_sokobanStage) {
    switch (enum_sokobanStage) {
        case stage_game:
            current_handler = stageGame_handle;
            stageGame_handle(EVT_INIT, 0, 0);
            stageGame_handle(EVT_SHOW, 0, 0);
            break;
        case stage_start:
            current_handler = stageStart_handle;
            stageGame_handle(EVT_INIT, 0, 0);
            stageGame_handle(EVT_SHOW, 0, 0);
            break;
        default:
            return;
    }
}