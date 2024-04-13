#include "Elevator.h"
#include <ncurses.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include "../config/Utilities.h"

extern std::mutex mx_drawing;
extern int shaft_start_x, shaft_start_y;

void animateRectangle() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    int rect_height = 5, rect_width = 8;
    int start_x = shaft_start_x;
    int prev_y = shaft_start_y;

    shaft_start_y = 3;

    shaft_start_x = 1 + ((COLS - 10) / 2);
    init_pair(2, COLOR_BLACK, COLOR_GREEN);

    std::vector<int> floor_positions = {9, 20, 31};


    while (program_running.load()) {
        int target_floor = floor_positions[std::rand() % floor_positions.size()] + 3;

        WINDOW *elevator = newwin(rect_height, rect_width, shaft_start_y, start_x);

        {
            std::lock_guard <std::mutex> writing_lock(mx_drawing);
            mvwhline(elevator, 0, 1, ACS_HLINE, rect_width - 1); // Góra, pomijamy lewą krawędź
            mvwhline(elevator, rect_height - 1, 1, ACS_HLINE, rect_width - 1); // Dół, pomijamy lewą krawędź

            mvwvline(elevator, 0, rect_width - 1, ACS_VLINE, rect_height); // Prawa ściana

            mvwaddch(elevator, 0, rect_width - 1, ACS_URCORNER); // Górny prawy róg
            mvwaddch(elevator, rect_height - 1, rect_width - 1, ACS_LRCORNER); // Dolny prawy róg

            wrefresh(elevator);
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));

        for (int start_y = shaft_start_y; start_y <= target_floor; ++start_y) {
            if (!program_running) {
                break;
            }
            {
                std::lock_guard <std::mutex> writing_lock(mx_drawing);

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
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(400));
        }
        {
            std::lock_guard <std::mutex> writing_lock(mx_drawing);

            werase(elevator);
            mvwin(elevator, target_floor, start_x);

            mvwhline(elevator, 0, 0, ACS_HLINE, rect_width - 1);
            mvwhline(elevator, rect_height - 1, 0, ACS_HLINE, rect_width - 1);

            mvwvline(elevator, 0, 0, ACS_VLINE, rect_height);

            mvwaddch(elevator, 0, 0, ACS_ULCORNER);
            mvwaddch(elevator, rect_height - 1, 0, ACS_LLCORNER);

            wrefresh(elevator);
        }
        delwin(elevator);
    }

}
