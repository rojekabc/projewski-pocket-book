#include <inkview.h>
#include <string.h>
#include "stagemain.h"

int main() {
#ifdef PLATFORM_FW5
    OpenScreen();
    // This operation needs OpenScreen call before, or SEGV
    // And it disables
    SetPanelType(PANEL_DISABLED);
    // SetShowPanelReader(0);
    // SetPanelSeparatorEnabled(0);
#endif
    InkViewMain(stage_main_handler);
    return 0;
}
