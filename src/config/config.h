#ifndef CONFIG_H
#define CONFIG_H

#include <mutex>
#include <atomic>

#define FIRST_FLOOR 2

extern std::mutex mx_drawing;
extern std::atomic<bool> program_running;

#endif // CONFIG_H