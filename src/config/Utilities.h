#ifndef UTILITIES_H
#define UTILITIES_H

#include <ncurses.h>
#include <mutex>
#include <atomic>
#include <vector>
#include <condition_variable>

struct Passenger {
    char symbol;
    int color_pair;
};

extern std::atomic<bool> program_running;
extern std::mutex mx_drawing;
extern int shaft_start_x, shaft_start_y;
extern std::vector<Passenger> passengers;
extern std::atomic<bool> elevator_ready_to_enter;
extern std::atomic<int> exit_floor;
extern std::atomic<bool> elevator_ready_to_exit;
extern std::mutex mx_elevator;
extern std::condition_variable cv_elevator_enter;
extern std::condition_variable cv_elevator_exit;

WINDOW *initializeExitWindow();

void exitTask();


#endif // UTILITIES_H
