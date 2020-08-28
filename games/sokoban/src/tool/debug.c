#ifdef DEBUG
#include <stdio.h>
#include <inkview.h>

#define DEBUG_FILE GAMEPATH "/sokoban.out"

FILE* debugFile = NULL;

FILE* debug_file() {
    if (debugFile == NULL) {
        debugFile = fopen(DEBUG_FILE, "a");
    }
    return debugFile;
}

void debug(const char* format, ...) {
    FILE* file = debug_file();
    if (file == NULL) {
        return;
    }

    va_list argPointer;
    va_start(argPointer, format);
    vfprintf(file, format, argPointer);
    va_end(argPointer);

    fflush(file);
}

#endif