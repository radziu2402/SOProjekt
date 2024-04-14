#include "Utilities.h"
#include <ncurses.h>
#include <mutex>
#include <string>

using namespace std;

std::mutex mx_drawing;
int shaft_start_x, shaft_start_y;
std::atomic<bool> program_running(true);
std::vector<Passenger> passengers;
std::atomic<bool> elevator_ready_to_enter;
std::atomic<bool> elevator_ready_to_exit;


WINDOW *initializeExitWindow() {
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    const int height = 3;
    const int width = 30;
    const int start_x = (COLS - width) / 2;

    WINDOW *exit_window = newwin(height, width, 0, start_x);
    std::lock_guard <std::mutex> writing_lock(mx_drawing);
    wattron(exit_window, COLOR_PAIR(1));
    box(exit_window, 0, 0);
    wattroff(exit_window, COLOR_PAIR(1));

    const string message = "Press SPACE to exit";
    mvwprintw(exit_window, height / 2, (width - message.size()) / 2, "%s", message.c_str());

    wrefresh(exit_window);
    return exit_window;
}

void exitTask() {
    WINDOW *exit_window = initializeExitWindow();

    while (true) {
        int input_char = wgetch(exit_window);
        if (input_char == ' ') {
            program_running = false;
            break;
        }
    }
    delwin(exit_window);
}
