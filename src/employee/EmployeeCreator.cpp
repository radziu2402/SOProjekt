#include "EmployeeCreator.h"
#include <thread>
#include <chrono>
#include "../config/Utilities.h"
#include "Employee.h"

void workerCreator() {
    while (program_running.load()) {
        std::thread worker(startEmployeeSimulation);
        worker.detach();
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
}