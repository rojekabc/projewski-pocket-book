#include "tool/array.h"

typedef struct Collection {
	char* folder;
	char* author;
	char* title;
	int levels;
} Collection;

GOC_Array* collections_retrieve();
