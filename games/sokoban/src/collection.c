#include <dirent.h>
#include <inkview.h>

#include "malloc.h"
#include "array.h"
#include "mystr.h"
#include "collection.h"

#ifndef EMULATOR
#	define levelsFolder USERDATA "/share/sokoban/"
#else
#	define levelsFolder "SokobanLevels/"
#endif

GOC_Array* collections = NULL;

struct Collection* collection_alloc() {
	ALLOC(Collection, collection);
	return collection;
}

void collection_free(struct Collection* collection) {
	if (collection == NULL) {
		return;
	}

	string_free(collection->folder);
	string_free(collection->author);
	string_free(collection->title);
	free(collection);
}

struct Collection* collection_retrieve(const char* name) {
	struct Collection* result = collection_alloc();
	string_add(result->folder, levelsFolder);
	string_add(result->folder, name);
	DIR* dir = opendir(result->folder);
	char line[128];

	string_set(result->title, name);
	struct dirent *file;
	while ( (file = readdir(dir)) ) {
		if ( file->d_type != DT_REG ) {
			continue;
		}
		if ( goc_stringStartsWith(file->d_name, "level") ) {
			result->levels++;
			continue;
		}
		if ( goc_stringEquals(file->d_name, "global.desc") ) {
			char* filename = strdup(result->folder);
			string_add(filename, "/global.desc");
			FILE* descFile = fopen(filename, "r");
			while( (fgets(line, 128, descFile)) ) {
				if ( goc_stringStartsWith(line, "Author: ") ) {
					string_set(result->author, line + 8);	
				} else if ( goc_stringStartsWith(line, "Title: ") ) {
					string_set(result->title, line + 7);	
				}
			}
			fclose(descFile);
		}
	}
	closedir(dir);
	if ( result->levels == 0 ) {
		collection_free(result);
		return NULL;
	}
	return result;
}

GOC_Array* collections_retrieve() {
	if (collections) {
		return collections;
	}

	collections = goc_arrayAlloc();
	DIR* dir = opendir(levelsFolder);
	if (dir == NULL ) {
		return collections;
	}


	struct dirent *file;
	struct Collection* collection;
	while ( (file = readdir(dir)) ) {
		if (file->d_name[0] == '.') {
			continue;
		}
		if (file->d_type != DT_DIR) {
			continue;
		}
		collection = collection_retrieve(file->d_name);
		if (collection) {
			array_add(collections, collection);
		}
	}
	closedir(dir);

	return collections;
}
