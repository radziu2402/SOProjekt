#include "Elevator.h"
#include "mutex"
#include "../config/config.h"

//void Elevator::draw_elevator() {
//    int max_x, max_y;
//    int width = 40;
//    int height = 5;
//
//    // Initialize color pair
//    start_color();
//    init_pair(1, COLOR_BLUE, COLOR_BLACK);
//    use_default_colors(); // Use the terminal's default colors
//
//    // Get maximum dimensions of the terminal
//    getmaxyx(stdscr, max_y, max_x);
//
//    // Coordinates for the top left corner
//    int start_x = 55;
//    int start_y = 3;
//
//    std::lock_guard<std::mutex> writing_lock(mx_drawing);
//
//    // Create a new window for the elevator at the top left corner
//    WINDOW* elevator_window = newwin(height, width, start_y, start_x);
//
//    // Lock for safe drawing if using multiple threads
//
//    // Create a box around the window with the defined border characters
//    box(elevator_window, 0, 0);
//
//    // Apply color pair to the window and then turn it off
//    wattron(elevator_window, COLOR_PAIR(1));
//    // You might want to add some content to the window here
//
//    wattroff(elevator_window, COLOR_PAIR(1));
//
//    // Refresh the window to show changes
//    wrefresh(elevator_window);
//
//    // Remember to delete the window when done to prevent memory leaks
//    // delwin(elevator_window);
//}

//
//void Elevator::draw_elevator_shaft() {
//     int max_x, max_y;
//    getmaxyx(stdscr, max_y, max_x);
//
//    const int height = 40;
//    const int width = 20;
//    const int start_y = 3;
//    const int start_x = (COLS - width) / 2;
//
//    WINDOW *elevator_shaft = newwin(height, width, start_y, start_x);
//
//    std::lock_guard<std::mutex> writing_lock(mx_drawing);
//    box(elevator_shaft, 0, 0);
//
//    wrefresh(elevator_shaft);
//}