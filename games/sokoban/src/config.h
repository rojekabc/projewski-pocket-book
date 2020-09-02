#include "inkview.h"
#ifndef EMULATOR
#	define configFileName STATEPATH "/sokoban.cfg"
#	define levelsFolder USERDATA "/share/sokoban/"
#else
#	define configFileName "sokoban.cfg"
#	define levelsFolder "SokobanLevels/"
#endif

void config_setLevel(const char* collectionName, int levelNo);
int config_getLevel();
char* config_getLevelFileName();
const char* config_getCollectionName();
void config_setCollection(const char *collectionName);
const char* config_slot_getName();
void config_slot_select(int pos);
void configuration_store();
