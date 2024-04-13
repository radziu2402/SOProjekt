#include <ncurses.h>
#include <string>
#include <iostream>
#include <thread>
#include <mutex>
#include "config/config.h"
#include <cstdlib>
#include <ctime>
#include <vector>
#include <chrono>

using namespace std;

int shaft_start_y;
int shaft_height;
int shaft_start_x;
int corridor_start_x;
int corridor_level_y;

WINDOW *initialize_exit_window() {
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    const int height = 3;
    const int width = 30;
    const int start_x = (COLS - width) / 2;

    WINDOW *exit_window = newwin(height, width, 0, start_x);
    std::lock_guard<std::mutex> writing_lock(mx_drawing);
    wattron(exit_window, COLOR_PAIR(1));
    box(exit_window, 0, 0);
    wattroff(exit_window, COLOR_PAIR(1));

    const string message = "Press SPACE to exit";
    mvwprintw(exit_window, height / 2, (width - message.size()) / 2, "%s", message.c_str());

    wrefresh(exit_window);
    return exit_window;
}

void exit_task() {
    WINDOW *exit_window = initialize_exit_window();

    while (true) {
        int input_char = wgetch(exit_window);
        if (input_char == ' ') {
            program_running = false;
            break;
        }
    }
    delwin(exit_window);
}

void draw_rectangle(WINDOW *win, int y, int x, int height, int width) {
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (i == 0 || i == height - 1 || j == 0 || j == width - 1) {
                mvwaddch(win, y + i, x + j, '*');
            } else {
                mvwaddch(win, y + i, x + j, ' ');
            }
        }
    }
    wrefresh(win);
}

void animate_rectangle() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    int rect_height = 5, rect_width = 8;
    int start_x = shaft_start_x;
    int prev_y = shaft_start_y;


    init_pair(2, COLOR_BLACK, COLOR_GREEN);

    std::vector<int> floor_positions = {9, 20, 31};


    while (program_running.load()) {
        int target_floor = floor_positions[std::rand() % floor_positions.size()] + 3;

        WINDOW *elevator = newwin(rect_height, rect_width, shaft_start_y, start_x);

        {
            std::lock_guard<std::mutex> writing_lock(mx_drawing);
            mvwhline(elevator, 0, 1, ACS_HLINE, rect_width - 1); // Góra, pomijamy lewą krawędź
            mvwhline(elevator, rect_height - 1, 1, ACS_HLINE, rect_width - 1); // Dół, pomijamy lewą krawędź

            mvwvline(elevator, 0, rect_width - 1, ACS_VLINE, rect_height); // Prawa ściana

            mvwaddch(elevator, 0, rect_width - 1, ACS_URCORNER); // Górny prawy róg
            mvwaddch(elevator, rect_height - 1, rect_width - 1, ACS_LRCORNER); // Dolny prawy róg

            wrefresh(elevator);
        }

        std::this_thread::sleep_for(std::chrono::seconds(5));

        for (int start_y = shaft_start_y; start_y <= target_floor; ++start_y) {
            if (!program_running) {
                break;
            }

            std::lock_guard<std::mutex> writing_lock(mx_drawing);

            if (start_y == 9) {
                WINDOW *elevator_no_left_wall = newwin(rect_height, rect_width, shaft_start_y, start_x);

                // Rysowanie górnej i dolnej linii ramki
                mvwhline(elevator_no_left_wall, 0, 1, ACS_HLINE, rect_width - 1); // Góra, pomijamy lewą krawędź
                mvwhline(elevator_no_left_wall, rect_height - 1, 1, ACS_HLINE,
                         rect_width - 1); // Dół, pomijamy lewą krawędź

                // Rysowanie prawej linii ramki
                mvwvline(elevator_no_left_wall, 0, rect_width - 1, ACS_VLINE, rect_height); // Prawa ściana

                // Rysowanie rogów
                mvwaddch(elevator_no_left_wall, 0, rect_width - 1, ACS_URCORNER); // Górny prawy róg
                mvwaddch(elevator_no_left_wall, rect_height - 1, rect_width - 1, ACS_LRCORNER); // Dolny prawy róg

                wrefresh(elevator_no_left_wall);

                // Pamiętaj, aby zwolnić zasoby po zakończeniu używania tego okna
                delwin(elevator_no_left_wall);
            }

            werase(elevator);
            mvwin(elevator, prev_y, start_x);
            wrefresh(elevator);

            mvwin(elevator, start_y, start_x);

            box(elevator, 0, 0);
            wrefresh(elevator);

            prev_y = start_y;

            std::this_thread::sleep_for(std::chrono::milliseconds(400));
        }
        {
            std::lock_guard<std::mutex> writing_lock(mx_drawing);

            // Przygotowanie do narysowania windy bez prawej ściany
            werase(elevator); // Najpierw wyczyść aktualne okno, żeby przygotować do rysowania nowej ramki
            mvwin(elevator, target_floor, start_x); // Przesuń okno windy na docelowe piętro

            // Rysowanie górnej i dolnej linii ramki bez prawej ściany
            mvwhline(elevator, 0, 0, ACS_HLINE, rect_width - 1); // Góra, rysujemy całą linię
            mvwhline(elevator, rect_height - 1, 0, ACS_HLINE, rect_width - 1); // Dół, rysujemy całą linię

            // Rysowanie lewej linii ramki
            mvwvline(elevator, 0, 0, ACS_VLINE, rect_height); // Lewa ściana

            // Rysowanie rogów bez prawych rogów
            mvwaddch(elevator, 0, 0, ACS_ULCORNER); // Górny lewy róg
            mvwaddch(elevator, rect_height - 1, 0, ACS_LLCORNER); // Dolny lewy róg

            wrefresh(elevator); // Odświeżamy okno, żeby pokazać zmiany
        }
        delwin(elevator);
    }

}


void draw_box_no_left(WINDOW *win, int height, int width) {
    mvwhline(win, 0, 0, ACS_HLINE, width);
    mvwhline(win, height - 1, 0, ACS_HLINE, width);

    mvwvline(win, 0, width - 1, ACS_VLINE, height);

    mvwaddch(win, 0, width - 1, ACS_URCORNER);
    mvwaddch(win, height - 1, width - 1, ACS_LRCORNER);
}


void draw_elevator_shaft_and_corridors() {
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
    const int level_zero_y = LINES - level_zero_height - 1;
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

        draw_box_no_left(corridor, corridor_height, corridor_width);

        wrefresh(corridor);
        delwin(corridor);
    }

    wrefresh(elevator_shaft);
    delwin(elevator_shaft);
}


void draw_elevator() {
    int max_x, max_y;
    int width = 50;
    int height = 5;

    start_color();
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    use_default_colors();

    getmaxyx(stdscr, max_y, max_x);

    int start_x = 45;
    int start_y = 3;

    std::lock_guard<std::mutex> writing_lock(mx_drawing);

    WINDOW *elevator_window = newwin(height, width, start_y, start_x);
    box(elevator_window, 0, 0);

    wattron(elevator_window, COLOR_PAIR(1));
    wattroff(elevator_window, COLOR_PAIR(1));
    wrefresh(elevator_window);

    delwin(elevator_window);
}

int main() {
    initscr();
    if (stdscr == nullptr) {
        return -1;
    }
    start_color();
    noecho();
    cbreak();
    curs_set(0);
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    shaft_start_y = 3;
    shaft_height = 40;
    shaft_start_x = 1 + ((COLS - 10) / 2);
    draw_elevator();
    draw_elevator_shaft_and_corridors();

    thread t1(exit_task);
    thread t2(animate_rectangle);

    corridor_start_x = shaft_start_x - 43;
    corridor_level_y = shaft_start_y + 3;

    t1.join();
    t2.join();

    endwin();
    return 0;
}