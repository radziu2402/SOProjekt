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
    drawZeroFloorCorridor();
    drawElevatorShaftAndCorridors();
    thread t1(exitTask);
    thread t2(animateRectangle);
    thread employeeThread(startEmployeeSimulation);


    t1.join();
    t2.join();
    employeeThread.join();

    endwin();
    return 0;
}