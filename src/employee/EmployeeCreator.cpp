#include "EmployeeCreator.h"
#include <thread>
#include <chrono>
#include "../config/Utilities.h"
#include "Employee.h"
#include <atomic>


void workerCreator() {
    while (program_running.load()) {
        int employee_id = next_employee_id++;
        std::thread worker(startEmployeeSimulation, employee_id);
        worker.detach();
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}