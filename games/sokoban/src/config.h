#include "inkview.h"
#ifndef EMULATOR
#	define configFileName STATEPATH "/sokoban.cfg"
#	define levelsFolder USERDATA "/share/sokoban/"
#else
#	define configFileName "sokoban.cfg"
#	define levelsFolder "SokobanLevels/"
#endif

void config_write();
void config_read();
void config_setLevel(int level);
void config_setLevelFileName(char* filename);
int config_getLevel();
char* config_getLevelFileName();
