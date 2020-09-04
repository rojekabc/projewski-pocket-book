#include "inkview.h"

#ifndef EMULATOR
#	define configFileName STATEPATH "/sokoban.cfg"
#	define levelsFolder USERDATA "/share/sokoban/"
#else
#	define configFileName "sokoban.cfg"
#	define levelsFolder "SokobanLevels/"
#endif

void config_setLevel(const char *collectionName, int levelNo);

int config_getLevel();

char *config_getLevelFileName();

const char *config_getCollectionName();

void config_setCollection(const char *collectionName);

const char *config_slot_getName();

void config_slot_select(int pos);

void configuration_store();

const char *config_getUsername();

const char *config_getSlotUsername(int slot);

void config_setUsername(const char *username);

const char *config_getPassword();

void config_setPassword(const char *password);
