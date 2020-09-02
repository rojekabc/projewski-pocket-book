#include <stdio.h>
#include <malloc.h>

#include "tool/fileostream.h"
#include "tool/fileistream.h"
#include "tool/properties.h"
#include "tool/mystr.h"
#include "config.h"
#include "tool/malloc.h"
#include "tool/debug.h"

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

#define SLOT_AMOUNT 4
#define SLOT_ANONYMOUS_ID 3

typedef struct {
    char *username;
    char *collection;
    int level;
    const char *slotCategory;
    char *levelFilename;
} SlotConfiguration;

typedef struct {
    SlotConfiguration slotConfiguration[4];
    SlotConfiguration *currentConfiguration;
} Configuration;

Configuration *configuration = NULL;
GOC_Properties *properties = NULL;

char *calculateLevelFilename(const char *collectionName, int levelNumber);

const char *slot_category(int pos);

void configuration_load(Configuration *config);

Configuration *configuration_retrieve_default() {
    ALLOC(Configuration, result);
    for (int i = 0; i < SLOT_AMOUNT; i++) {
        result->slotConfiguration[i].level = 1;
        result->slotConfiguration[i].collection = strdup(DEFAULT_COLLECTION);
        result->slotConfiguration[i].slotCategory = slot_category(i);
    }
    result->currentConfiguration = &(result->slotConfiguration[SLOT_ANONYMOUS_ID]);
    result->currentConfiguration->username = strdup("Anonymous");
    return result;
}

Configuration *configuration_retrieve() {
    if (configuration == NULL) {
        configuration = configuration_retrieve_default();
        configuration_load(configuration);
    }
    return configuration;
}

void configuration_store() {
    if (configuration == NULL) {
        return;
    }
    goc_propertiesCategorySet(properties, NULL, "collection",
                              configuration->slotConfiguration[SLOT_ANONYMOUS_ID].collection);
    char *levelString = goc_stringAddInt(NULL, configuration->slotConfiguration[SLOT_ANONYMOUS_ID].level);
    goc_propertiesCategorySet(properties, NULL, configuration->slotConfiguration[SLOT_ANONYMOUS_ID].collection,
                              levelString);
    string_free(levelString);
    for (int i = 0; i < SLOT_AMOUNT - 1; i++) {
        if (configuration->slotConfiguration[i].username == NULL) {
            continue;
        }
        goc_propertiesCategorySet(properties, configuration->slotConfiguration[i].slotCategory, "collection",
                                  configuration->slotConfiguration[i].collection);
        levelString = goc_stringAddInt(NULL, configuration->slotConfiguration[i].level);
        goc_propertiesCategorySet(properties, configuration->slotConfiguration[i].slotCategory,
                                  configuration->slotConfiguration[i].collection, levelString);
        string_free(levelString);
    }

    GOC_OStream *oStream = goc_fileOStreamOpen(configFileName);
    if (oStream != NULL) {
        goc_propertiesSave(properties, oStream);
        goc_osClose(oStream);
    }
}

void configuration_load(Configuration *config) {
    if (config == NULL) {
        return;
    }

    GOC_IStream *iStream = goc_fileIStreamOpen(configFileName);
    if (iStream == NULL) {
        debug("Cannot open configuration file %s\n", configFileName);
        return;
    }

    properties = goc_propertiesLoad(properties, iStream);
    goc_isClose(iStream);
    for (int i = 0; i < SLOT_AMOUNT; i++) {
        const char *slotCategory = config->slotConfiguration[i].slotCategory;
        const char *username = goc_propertiesGetCategoryValue(properties, slotCategory, "username");
        if (username == NULL && i != SLOT_ANONYMOUS_ID) {
            continue;
        }
        const char *collection = goc_propertiesGetCategoryValue(properties, slotCategory, "collection");
        if (collection) {
            string_set(config->slotConfiguration[i].collection, collection);
            const char *collectionLevel = goc_propertiesGetCategoryValue(properties, slotCategory, collection);
            config->slotConfiguration[i].level = (collectionLevel ? atoi(collectionLevel) : 1);
            debug("Set slot %d to %s:%d\n", i, config->slotConfiguration[i].collection, config->slotConfiguration[i].level);
        } else {
            debug("Collection is NULL for slot %d", i);
        }
    }
}

void config_setLevel(const char *collectionName, int level) {
    SlotConfiguration *currentConfiguration = configuration_retrieve()->currentConfiguration;
    string_set(currentConfiguration->collection, collectionName ? collectionName : DEFAULT_COLLECTION);
    currentConfiguration->level = level;
    goc_propertiesCategorySet(properties, currentConfiguration->slotCategory, "collection", collectionName);
    char *levelString = goc_stringAddInt(NULL, level);
    goc_propertiesCategorySet(properties, currentConfiguration->slotCategory, collectionName, levelString);
    goc_stringFree(levelString);
}

void config_setCollection(const char *collectionName) {
    if (collectionName == NULL) {
        collectionName = DEFAULT_COLLECTION;
    }

    SlotConfiguration *currentConfiguration = configuration_retrieve()->currentConfiguration;
    const char *stringLevel = goc_propertiesGetCategoryValue(properties, currentConfiguration->slotCategory,
                                                             collectionName);
    if (stringLevel == NULL) {
        config_setLevel(collectionName, 1);
    } else {
        config_setLevel(collectionName, atoi(stringLevel));
    }
}

int config_getLevel() {
    return configuration_retrieve()->currentConfiguration->level;
}

char *config_getLevelFileName() {
    SlotConfiguration *currentConfiguration = configuration_retrieve()->currentConfiguration;
    return string_assign(currentConfiguration->levelFilename,
                  calculateLevelFilename(currentConfiguration->collection, currentConfiguration->level));
}

const char *config_getCollectionName() {
    return configuration_retrieve()->currentConfiguration->collection;
}

void config_slot_select(int pos) {
    Configuration *config = configuration_retrieve();
    config->currentConfiguration = &(config->slotConfiguration[pos]);
}

const char *config_slot_getName() {
    char *username = configuration_retrieve()->currentConfiguration->username;
    return username ? username : configuration_retrieve()->currentConfiguration->slotCategory;
}

const char *slot_category(int pos) {
    switch (pos) {
        case 0:
            return "slot1";
        case 1:
            return "slot2";
        case 2:
            return "slot3";
        default:
            return NULL;
    }
}

char *calculateLevelFilename(const char *collectionName, int levelNumber) {
    char *levelFilename = NULL;
    string_set(levelFilename, levelsFolder);
    string_add(levelFilename, collectionName);
    if (levelNumber < 10) {
        string_add(levelFilename, "/level00");
    } else if (levelNumber < 100) {
        string_add(levelFilename, "/level0");
    } else {
        string_add(levelFilename, "/level");
    }
    return string_addInt(levelFilename, levelNumber);
}