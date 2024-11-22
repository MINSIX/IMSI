#include "robot_moving_event.h"

int main() {
    int startX = 4, startY = 6; // 시작 위치
    int goalTableNum = 14;
    aStar(startX, startY, goalTableNum);
    return 0;
}