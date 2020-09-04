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
    char *password;
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
    for (int i = 0; i < SLOT_AMOUNT; i++) {
        if (configuration->slotConfiguration[i].username == NULL) {
            continue;
        }
        if (i != SLOT_ANONYMOUS_ID) {
            debug("Set up user %s for slot %d with category %s\n", configuration->slotConfiguration[i].username, i,
                  configuration->slotConfiguration[i].slotCategory);
            goc_propertiesCategorySet(properties, configuration->slotConfiguration[i].slotCategory, "username",
                                      configuration->slotConfiguration[i].username);
            goc_propertiesCategorySet(properties, configuration->slotConfiguration[i].slotCategory, "password",
                                      configuration->slotConfiguration[i].password);
        }
        debug("Set up collection %s for slot %d with category %s\n", configuration->slotConfiguration[i].collection, i,
              configuration->slotConfiguration[i].slotCategory);
        goc_propertiesCategorySet(properties, configuration->slotConfiguration[i].slotCategory, "collection",
                                  configuration->slotConfiguration[i].collection);
        char *levelString = goc_stringAddInt(NULL, configuration->slotConfiguration[i].level);
        goc_propertiesCategorySet(properties, configuration->slotConfiguration[i].slotCategory,
                                  configuration->slotConfiguration[i].collection, levelString);
        string_free(levelString);
    }

    GOC_OStream *oStream = goc_fileOStreamOpen(configFileName);
    if (oStream != NULL) {
        debug("Properties save");
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
        if (i != SLOT_ANONYMOUS_ID) {
            string_set(config->slotConfiguration[i].username, username);
            string_set(config->slotConfiguration[i].password,
                       goc_propertiesGetCategoryValue(properties, slotCategory, "password"));
        }
        const char *collection = goc_propertiesGetCategoryValue(properties, slotCategory, "collection");
        if (collection) {
            string_set(config->slotConfiguration[i].collection, collection);
            const char *collectionLevel = goc_propertiesGetCategoryValue(properties, slotCategory, collection);
            config->slotConfiguration[i].level = (collectionLevel ? atoi(collectionLevel) : 1);
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

const char *config_getUsername() {
    return configuration_retrieve()->currentConfiguration->username;
}

void config_setUsername(const char *username) {
    string_set(configuration_retrieve()->currentConfiguration->username, username);
}

const char *config_getSlotUsername(int slot) {
    return configuration_retrieve()->slotConfiguration[slot].username;
}

const char *config_getPassword() {
    return configuration_retrieve()->currentConfiguration->password;
}

void config_setPassword(const char *password) {
    string_set(configuration_retrieve()->currentConfiguration->password, password);
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