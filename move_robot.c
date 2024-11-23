#include <stdio.h>
#include "robot_moving_event.h"

void move() {

}

void startMove() {
    printf("start move\n");
    while(1) {
        if (isEmpty(&moveCommandQueue)) {
            printf("empty! stop move!\n");
            break;
        }
        
        printf("dequeue direction\n");
        int* task = dequeue(&moveCommandQueue);
        printf("%d ->", (int)(*task));
        // 커맨드대로 진행
        // 복귀 명령이 올 때 까지 무한 대기? or 일정 시간 지나고 복귀
    }
    printf("end move\n");
}