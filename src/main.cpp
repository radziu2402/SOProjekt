#include <ncurses.h>
#include <string>
#include <iostream>
#include <thread>
#include <mutex>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <chrono>
#include "config/Utilities.h"
#include "elevator/Elevator.h"
#include "elevator/ElevatorShaft.h"
#include "employee/Employee.h"

using namespace std;

int main() {
    initscr();
    if (stdscr == nullptr) {
        return -1;
    }
    start_color();
    noecho();
    cbreak();
    curs_set(0);

    shaft_start_y = 3;
    shaft_start_x = 1 + ((COLS - 10) / 2);
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_RED);


    drawZeroFloorCorridor();
    drawElevatorShaftAndCorridors();

    thread t1(exitTask);
    thread t2(animateRectangle);
    std::vector<std::thread> workers;
    for (int i = 0; i < 50; ++i) {
        workers.emplace_back(startEmployeeSimulation);
    }

    for (auto& worker : workers) {
        worker.join();
    }

    t1.join();
    t2.join();

    endwin();
    return 0;
}