#include "Elevator.h"
#include <ncurses.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include "../config/Utilities.h"
#include <atomic>
#include <vector>

extern std::mutex mx_drawing;
extern int shaft_start_x, shaft_start_y;

void animateElevator() {
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
        exit_floor = target_floor;
        WINDOW *elevator = newwin(rect_height, rect_width, shaft_start_y, start_x);

        {
            std::lock_guard <std::mutex> writing_lock(mx_drawing);
            mvwhline(elevator, 0, 1, ACS_HLINE, rect_width - 1);
            mvwhline(elevator, rect_height - 1, 1, ACS_HLINE, rect_width - 1);

            mvwvline(elevator, 0, rect_width - 1, ACS_VLINE, rect_height);

            mvwaddch(elevator, 0, rect_width - 1, ACS_URCORNER);
            mvwaddch(elevator, rect_height - 1, rect_width - 1, ACS_LRCORNER);

            wrefresh(elevator);
        }

        // Sygnalizujemy gotowość windy do wejścia
        {
            std::lock_guard<std::mutex> lock(mx_elevator);
            elevator_ready_to_enter = true;
        }
        cv_elevator_enter.notify_all();

        std::this_thread::sleep_for(std::chrono::seconds(5));

        // Zamykamy możliwość wejścia do windy
        {
            std::lock_guard<std::mutex> lock(mx_elevator);
            elevator_ready_to_enter = false;
        }

        for (int start_y = shaft_start_y; start_y <= target_floor; ++start_y) {
            if (!program_running) {
                break;
            }
            {
                std::lock_guard <std::mutex> writing_lock(mx_drawing);
                if (start_y == 9) {
                    WINDOW *elevator_no_left_wall = newwin(rect_height, rect_width, shaft_start_y, start_x);

                    mvwhline(elevator_no_left_wall, 0, 1, ACS_HLINE, rect_width - 1);
                    mvwhline(elevator_no_left_wall, rect_height - 1, 1, ACS_HLINE,
                             rect_width - 1);

                    mvwvline(elevator_no_left_wall, 0, rect_width - 1, ACS_VLINE, rect_height);

                    mvwaddch(elevator_no_left_wall, 0, rect_width - 1, ACS_URCORNER);
                    mvwaddch(elevator_no_left_wall, rect_height - 1, rect_width - 1, ACS_LRCORNER);
                    wrefresh(elevator_no_left_wall);
                }
                werase(elevator);
                int passenger_y = 1;
                int passenger_x = 1;
                mvwin(elevator, prev_y, start_x);
                wrefresh(elevator);

                mvwin(elevator, start_y, start_x);

                for (const auto& passenger : passengers){
                    wattron(elevator, COLOR_PAIR(passenger.color_pair));
                    mvwprintw(elevator, passenger_y, passenger_x, "%c", passenger.symbol);
                    wattroff(elevator, COLOR_PAIR(passenger.color_pair));

                    if (passenger_x == 6) {
                        if(passenger_y == 3){
                            break;
                        }
                        passenger_x = 1;
                        passenger_y += 1;
                    } else {
                        passenger_x += 1;
                    }
                }

                box(elevator, 0, 0);
                wrefresh(elevator);

                prev_y = start_y;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(400));
        }
        exit_floor = target_floor;
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
            passengers.clear();

            {
                std::lock_guard<std::mutex> lock(mx_elevator);
                elevator_ready_to_exit = true;
            }
            cv_elevator_exit.notify_all();
        }

        std::this_thread::sleep_for(std::chrono::seconds(5));

        {
            std::lock_guard<std::mutex> lock(mx_elevator);
            elevator_ready_to_exit = false;
        }

        delwin(elevator);
    }
}
