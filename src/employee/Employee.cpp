#include <ncurses.h>
#include <thread>
#include <chrono>
#include "Employee.h"
#include <mutex>

extern std::mutex mx_drawing;
extern int shaft_start_x, shaft_start_y;

void startEmployeeSimulation() {
    int employee_width = 3;
    int employee_height = 1;
    int employee_start_x = 46; // Startowa pozycja pracownika
    int employee_start_y = 6;

    int target_x = shaft_start_x - 1; // Cel, na przykład 2 pozycje przed szybem windy

    WINDOW* employee_window = newwin(employee_height, employee_width, employee_start_y, employee_start_x);

    for (int i = 0; i < 40; ++i) {
        {
            std::lock_guard<std::mutex> guard(mx_drawing);
            werase(employee_window);
            mvwin(employee_window, employee_start_y, ++employee_start_x);
            mvwprintw(employee_window, 0, 1, "P");
            wrefresh(employee_window);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));

    delwin(employee_window);
}