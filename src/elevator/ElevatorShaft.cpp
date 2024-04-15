#include <ncurses.h>
#include "ElevatorShaft.h"
#include <mutex>
#include "../config/Utilities.h"

extern int shaft_start_x, shaft_start_y;

void drawElevatorShaftAndCorridors() {
    int max_x, max_y;
    getmaxyx(stdscr, max_y, max_x);

    const int shaft_height = 40;
    const int shaft_width = 10;
    const int shaft_start_x = (COLS - shaft_width) / 2;
    const int shaft_start_y = 3;

    const int corridor_width = 40;
    const int corridor_height = 5;
    const int start_x_corridor = shaft_start_x + shaft_width;
    const int start_y_offset = 6;

    const int level_zero_height = 3;
    const int level_zero_y = 49 - level_zero_height - 1;
    WINDOW *elevator_shaft = newwin(shaft_height, shaft_width, shaft_start_y, shaft_start_x);
    box(elevator_shaft, 0, 0);
    mvwaddch(elevator_shaft, 0, 0, ACS_HLINE);
    for (int y = 0; y < level_zero_height; ++y) {
        mvwaddch(elevator_shaft, y + 1, 0, ' ');
    }
    mvwaddch(elevator_shaft, 4, 0, ACS_HLINE);

    wrefresh(elevator_shaft);
    for (int i = 0; i < 3; ++i) {
        int start_y_corridor = level_zero_y - (corridor_height * (i + 1)) - start_y_offset - (i * start_y_offset);

        for (int y = start_y_corridor; y < start_y_corridor + corridor_height; ++y) {
            mvwaddch(elevator_shaft, y - shaft_start_y, shaft_width - 1, ' ');
            if (y == start_y_corridor) {
                mvwaddch(elevator_shaft, y - shaft_start_y, shaft_width - 1, ACS_HLINE);
            } else if (y == start_y_corridor + corridor_height - 1) {
                mvwaddch(elevator_shaft, y - shaft_start_y, shaft_width - 1, ACS_HLINE);
            }
        }

        WINDOW *corridor = newwin(corridor_height, corridor_width, start_y_corridor, start_x_corridor);

        drawBoxNoLeft(corridor, corridor_height, corridor_width);

        wrefresh(corridor);
        delwin(corridor);
    }

    wrefresh(elevator_shaft);
    delwin(elevator_shaft);
}

void drawZeroFloorCorridor() {
    int max_x, max_y;
    int width = 50;
    int height = 5;

    start_color();
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    use_default_colors();

    getmaxyx(stdscr, max_y, max_x);

    int start_x = COLS / 2 - 50;
    int start_y = 3;

    std::lock_guard <std::mutex> writing_lock(mx_drawing);

    WINDOW *elevator_window = newwin(height, width, start_y, start_x);
    box(elevator_window, 0, 0);

    wattron(elevator_window, COLOR_PAIR(1));
    wattroff(elevator_window, COLOR_PAIR(1));
    wrefresh(elevator_window);

    delwin(elevator_window);
}

void drawBoxNoLeft(WINDOW *win, int height, int width) {
    mvwhline(win, 0, 0, ACS_HLINE, width);
    mvwhline(win, height - 1, 0, ACS_HLINE, width);

    mvwvline(win, 0, width - 1, ACS_VLINE, height);

    mvwaddch(win, 0, width - 1, ACS_URCORNER);
    mvwaddch(win, height - 1, width - 1, ACS_LRCORNER);
}
