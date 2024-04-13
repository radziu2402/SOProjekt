#ifndef UTILITIES_H
#define UTILITIES_H

#include <ncurses.h>
#include <mutex>
#include <atomic>

extern std::atomic<bool> program_running;
extern std::mutex mx_drawing;
extern int shaft_start_x, shaft_start_y;

WINDOW *initializeExitWindow();

void exitTask();

#endif // UTILITIES_H
