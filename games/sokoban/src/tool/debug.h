#ifndef SOKOBAN_DEBUG_H
#define SOKOBAN_DEBUG_H

#ifdef DEBUG
void debug(const char* format, ...);
#else
#   define debug(...)
#endif

#endif //SOKOBAN_DEBUG_H
