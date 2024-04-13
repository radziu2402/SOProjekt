#ifndef UTILITIES_H
#define UTILITIES_H

#include <ncurses.h>
#include <mutex>

extern std::mutex mx_drawing; // Definiowanie tego mutexu globalnie, aby był dostępny w różnych częściach programu

WINDOW* initializeExitWindow();
void exitTask();

#endif // UTILITIES_H
