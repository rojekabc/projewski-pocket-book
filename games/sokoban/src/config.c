#include <stdio.h>
#include <malloc.h>

#include "config.h"

int levelNo;
char* levelSetFileName;

void config_write() {
	FILE *config = fopen(configFileName, "w");
	fprintf(config, "%i\n%s", levelNo, levelSetFileName);
	fclose(config);
}

void config_setLevel(int level) {
	levelNo = level;
}

void config_setLevelFileName(char* filename) {
	levelSetFileName = filename;
}

int config_getLevel() {
	return levelNo;
}

char* config_getLevelFileName() {
	return levelSetFileName;
}

void config_read() {
	FILE *config = fopen(configFileName, "r");

	if (config) {
		levelSetFileName = (char*)malloc(sizeof(char) * 1024);
		fscanf(config, "%i\n%s", &levelNo, levelSetFileName);
		fclose(config);
	} else {
		levelNo = 0;
		levelSetFileName = NULL;
	}
}
