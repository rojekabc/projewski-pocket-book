#include "tool/array.h"

typedef struct Collection {
	char* folder;
	char* author;
	char* title;
	int levels;
} Collection;

GOC_Array* collections_retrieve();
char* collection_createLevelFilename(struct Collection* collection, int levelNumber);
int collection_select(const char* collectionName);
Collection* collection_current();
