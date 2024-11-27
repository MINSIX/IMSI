#include <stdio.h>
#include "robot_moving_event.h"

void moveFront() {
    // 전진 속도?
    
}
void moveBack() {
    // 전진 속도?
}

// TODO : 좌회전과 우회전이 큐의 마지막 커맨드일 경우, 회전만 하고 앞으로 가지 않는다.
void turnLeft() {
    // 회전 속도?

}
void turnRight() {
    // 회전 속도?
}

void commandSelect(int command) {
    printf("command : %d\n", command);
    if (command == 1) {
        moveFront();
    } else if (command == 2) {
        moveBack();
    } else if (command == 3) {
        turnLeft();
    } else if (command == 4) {
        turnRight();
    } else {
        printf("Command not found");
        return;
    }
}

void startMove() {
    printf("start move\n");
    while(1) {
        if (isEmpty(&moveCommandQueue)) {
            printf("Destination Arrived\n");
            break;
        }
        
        printf("dequeue direction\n");
        int* task = dequeue(&moveCommandQueue);
        commandSelect((int)*task);

        // 1. 이동 전 가속도 센서를 통해 이동방향과 로봇이 향하고 있는 방향이 일치하는지 확인 후 각도를 변경
        // 2. 커맨드대로 라인 트레이싱하면서 진행
        // 3. 진행하면서 마커 인식을 통해 현재 row, col을 저장한다.
        // 4. 진행중 센싱 (초음파 센서를 통해 장애물이 앞에 있으면 멈추고 부저를 울린다. / 가속도 센서의 각이 갑자기 엇나가는 순간에 부저를 울린다.)
        // 5. 도착 후 복귀 명령이 올 때 까지 무한 대기? or 일정 시간 지나고 복귀
        // 6. 복귀는 최근 row, col을 기준으로 다시 astar를 통해 경로 계산 (일단 경로 중간에 복귀 버튼 눌리는 것은 생각하지 않는다.)
    }
    printf("end move\n");
}