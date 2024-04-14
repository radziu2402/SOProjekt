#include <ncurses.h>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <mutex>
#include <map>
#include <string>
#include <algorithm>
#include "../config/Utilities.h"


std::mutex mx_positions;
std::map<std::string, bool> occupiedPositions;

std::string makeKey(int x, int y) {
    return std::to_string(x) + "," + std::to_string(y);
}

bool isPositionFree(int x, int y) {
    std::lock_guard<std::mutex> lock(mx_positions);
    auto key = makeKey(x, y);
    return occupiedPositions.find(key) == occupiedPositions.end();
}

void occupyPosition(int x, int y) {
    std::lock_guard<std::mutex> lock(mx_positions);
    occupiedPositions[makeKey(x, y)] = true;
}

void freePosition(int x, int y) {
    std::lock_guard<std::mutex> lock(mx_positions);
    occupiedPositions.erase(makeKey(x, y));
}

void startEmployeeSimulation() {
    std::srand(std::time(nullptr) + std::hash<std::thread::id>{}(std::this_thread::get_id()));

    int employee_width = 2;
    int employee_height = 1;
    int employee_start_x =  COLS / 2 - 50;
    int waiting_for_elevator_x = COLS / 2 - 6;
    int employee_start_y = 4 + std::rand() % 3;

    char symbol = 'A' + std::rand() % 26;
    int color_pair_number = 1 + std::rand() % 3;
    int speed = 100 + (std::rand() % 5) * 50;

    WINDOW* employee_window = newwin(employee_height, employee_width, employee_start_y, employee_start_x);
    wattron(employee_window, COLOR_PAIR(color_pair_number));

    occupyPosition(employee_start_x, employee_start_y);

    while (true) {
        bool shouldMove = false;

        if (employee_start_x < waiting_for_elevator_x && isPositionFree(employee_start_x + 1, employee_start_y)) {
            occupyPosition(employee_start_x + 1, employee_start_y);
            freePosition(employee_start_x, employee_start_y);
            ++employee_start_x;
            shouldMove = true;
        }

        if (shouldMove) {
            std::lock_guard<std::mutex> guard(mx_drawing);
            werase(employee_window);
            mvwin(employee_window, employee_start_y, employee_start_x);
            mvwprintw(employee_window, 0, 1, "%c", symbol);
            wrefresh(employee_window);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(speed));
    }
}