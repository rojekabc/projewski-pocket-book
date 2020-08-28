#include <stdio.h>
#include <malloc.h>

#include "tool/fileostream.h"
#include "tool/fileistream.h"
#include "tool/properties.h"
#include "tool/mystr.h"
#include "config.h"

#ifndef EMULATOR
#	define levelsFolder USERDATA "/share/sokoban/"
#else
#	define levelsFolder "SokobanLevels/"
#endif
#define DEFAULT_COLLECTION "minicosmos"


/*
 * Configuration file:
 * # The last played level from collection
 * level.<collectionName>=<levelNumber>
 * # The name of the last played collection
 * collection=<collectionName>
 */

GOC_Properties *properties = NULL;
char *levelFilename = NULL;
const char *g_collectionName = NULL;
int levelNo = 1;

void updateLevelFilename(const char *collectionName, int levelNumber);

void config_write();

void config_read();

void config_setLevel(const char *collectionName, int level) {
    char *levelString = goc_stringAddInt(NULL, level);
    if (collectionName == NULL) {
        collectionName = DEFAULT_COLLECTION;
    }
    goc_propertiesSet(properties, "collection", collectionName);
    goc_propertiesSet(properties, collectionName, levelString);
    config_write();
    updateLevelFilename(collectionName, level);
    string_free(levelString);
}

void config_setCollection(const char *collectionName) {
    if (collectionName == NULL) {
        collectionName = DEFAULT_COLLECTION;
    }
    char *stringLevel = goc_propertiesGetValue(properties, collectionName);
    if (stringLevel == NULL) {
        config_setLevel(collectionName, 1);
    } else {
        config_setLevel(collectionName, atoi(stringLevel));
    }
}

int config_getLevel() {
    if (properties == NULL) {
        config_read();
    }
    return levelNo;
}

char *config_getLevelFileName() {
    if (properties == NULL) {
        config_read();
    }

    return levelFilename;
}

const char *config_getCollectionName() {
    if (properties == NULL) {
        config_read();
    }

    return g_collectionName;
}

// -- INTERNAL --
void config_write() {
    if (properties == NULL) {
        return;
    }

    GOC_OStream *oStream = goc_fileOStreamOpen(configFileName);
    if (oStream == NULL) {
        return;
    }

    goc_propertiesSave(properties, oStream);
    goc_osClose(oStream);
}

void config_read() {
    properties = goc_propertiesAlloc();
    GOC_IStream *iStream = goc_fileIStreamOpen(configFileName);
    if (iStream == NULL) {
        config_setLevel(DEFAULT_COLLECTION, 1);
        return;
    }
    properties = goc_propertiesLoad(NULL, iStream);
    goc_isClose(iStream);
    if (properties == NULL) {
        config_setLevel(DEFAULT_COLLECTION, 1);
    } else {
        char *collectionName = goc_propertiesGetValue(properties, "collection");
        int level = atoi(goc_propertiesGetValue(properties, collectionName));
        updateLevelFilename(collectionName, level);
    }
}

void updateLevelFilename(const char *collectionName, int levelNumber) {
    string_set(levelFilename, levelsFolder);
    string_add(levelFilename, collectionName);
    if (levelNumber < 10) {
        string_add(levelFilename, "/level00");
    } else if (levelNumber < 100) {
        string_add(levelFilename, "/level0");
    } else {
        string_add(levelFilename, "/level");
    }
    goc_stringAddInt(levelFilename, levelNumber);
    levelNo = levelNumber;
    g_collectionName = collectionName;
}