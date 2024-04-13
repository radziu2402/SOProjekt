#ifndef ELEVATORSHAFT_H
#define ELEVATORSHAFT_H

#include <ncurses.h>

class ElevatorShaft {
public:
    ElevatorShaft(int startX, int startY, int height, int width);
    void draw();
private:
    int startX, startY, height, width;
};

#endif // ELEVATORSHAFT_H
