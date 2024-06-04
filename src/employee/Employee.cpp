#include "Employee.h"
#include <ncurses.h>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <mutex>
#include <map>
#include <stack>
#include <string>
#include <algorithm>
#include "../config/Utilities.h"

std::mutex mx_positions;

struct ExitLocation {
    std::stack<int> waiting_employees;
    std::mutex mx_last_place;
    std::condition_variable cv_last_place_free;
    bool place_occupied = false;
};

std::map<int, ExitLocation> exit_locations;

void startEmployeeSimulation(int employee_id) {
    std::srand(std::time(nullptr) + std::hash<std::thread::id>{}(std::this_thread::get_id()));

    int employee_width = 2;
    int employee_height = 1;
    int employee_start_x = COLS / 2 - 50;
    int employee_start_x_after_exit = COLS / 2 + 1;
    int waiting_for_elevator_x = COLS / 2 - 6;
    int employee_start_y = 4 + std::rand() % 3;
    int waiting_for_disappear = COLS / 2 + 41;
    int our_exit_floor;

    char symbol = 'A' + std::rand() % 26;
    int color_pair_number = 1 + std::rand() % 10;
    int speed = 110 + (std::rand() % 5) * 50;

    WINDOW *employee_window = newwin(employee_height, employee_width, employee_start_y, employee_start_x);
    wattron(employee_window, COLOR_PAIR(color_pair_number));

    while (program_running.load()) {
        if (employee_start_x < waiting_for_elevator_x) {
            ++employee_start_x;
            {
                std::lock_guard<std::mutex> guard(mx_drawing);
                werase(employee_window);
                mvwin(employee_window, employee_start_y, employee_start_x);
                mvwprintw(employee_window, 0, 1, "%c", symbol);
                wrefresh(employee_window);
            }
        }
        if (employee_start_x == waiting_for_elevator_x) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(speed));
    }
    Passenger newPassenger{symbol, color_pair_number};

    {
        std::unique_lock<std::mutex> lock(mx_elevator);
        cv_elevator_enter.wait(lock, [] { return elevator_ready_to_enter.load(); });
    }

    {
        std::lock_guard<std::mutex> guard(mx_drawing);
        passengers.push_back(newPassenger);
        werase(employee_window);
        wrefresh(employee_window);
        delwin(employee_window);
        our_exit_floor = exit_floor;
    }

    std::this_thread::sleep_for(std::chrono::seconds(5));

    our_exit_floor = our_exit_floor + std::rand() % 3;
    int exit_location_key = our_exit_floor * 10 + (employee_start_x_after_exit % 3); // Unikalny klucz dla lokalizacji wyjścia

    {
        std::unique_lock<std::mutex> lock(mx_elevator);
        cv_elevator_exit.wait(lock, [] { return elevator_ready_to_exit.load(); });
    }

    WINDOW *employee_window_after_exit = newwin(employee_height, employee_width, our_exit_floor + 1, employee_start_x_after_exit);
    wattron(employee_window_after_exit, COLOR_PAIR(color_pair_number));

    while (program_running.load()) {
        if (exit_floor < waiting_for_elevator_x) {
            ++employee_start_x_after_exit;
            {
                std::lock_guard<std::mutex> guard(mx_drawing);
                werase(employee_window_after_exit);
                mvwin(employee_window_after_exit, our_exit_floor + 1, employee_start_x_after_exit + 1);
                mvwprintw(employee_window_after_exit, 0, 1, "%c", symbol);
                wrefresh(employee_window_after_exit);
            }
        }
        if (employee_start_x_after_exit == waiting_for_disappear - 1) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(speed));
    }

    bool should_wait = false;
    {
        std::lock_guard<std::mutex> lk(exit_locations[exit_location_key].mx_last_place);
        if (exit_locations[exit_location_key].place_occupied || !exit_locations[exit_location_key].waiting_employees.empty()) {
            should_wait = true;
            exit_locations[exit_location_key].waiting_employees.push(employee_id);
        } else {
            exit_locations[exit_location_key].place_occupied = true;
        }
    }

    if (should_wait) {
        {
            std::unique_lock<std::mutex> lk(exit_locations[exit_location_key].mx_last_place);
            if (exit_locations[exit_location_key].place_occupied) {
                exit_locations[exit_location_key].cv_last_place_free.wait(lk, [&] {
                    return (exit_locations[exit_location_key].waiting_employees.top() == employee_id && !exit_locations[exit_location_key].place_occupied) || !program_running.load();
                });
                if (!program_running.load()) {
                    return;
                }
                exit_locations[exit_location_key].place_occupied = true;
                exit_locations[exit_location_key].waiting_employees.pop();
            }
        }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(speed));
    ++employee_start_x_after_exit;
    if (!program_running.load()) {
        return;
    }
    {
        std::lock_guard<std::mutex> guard(mx_drawing);
        werase(employee_window_after_exit);
        mvwin(employee_window_after_exit, our_exit_floor + 1, employee_start_x_after_exit + 1);
        mvwprintw(employee_window_after_exit, 0, 1, "%c", symbol);
        wrefresh(employee_window_after_exit);
    }
    std::this_thread::sleep_for(std::chrono::seconds(5));

    {
        std::lock_guard<std::mutex> guard(mx_drawing);
        werase(employee_window_after_exit);
        wrefresh(employee_window_after_exit);
        delwin(employee_window_after_exit);
    }
    {
        std::lock_guard<std::mutex> lk(exit_locations[exit_location_key].mx_last_place);
        exit_locations[exit_location_key].place_occupied = false;
        if (!exit_locations[exit_location_key].waiting_employees.empty()) {
            if (!program_running.load()) {
                return;
            }
            exit_locations[exit_location_key].cv_last_place_free.notify_all();
        }
    }
}
