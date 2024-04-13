#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <ncurses.h>
#include <mutex>
#include <vector>

class Elevator {
public:
    Elevator(int startX, int startY, int height, int width);
    void animate();
private:
    int startX, startY, height, width;
    std::mutex mx_drawing;
};

#endif // ELEVATOR_H
