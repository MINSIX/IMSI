#include <stdio.h>
#include <stdlib.h>
#include "robot_moving_event.h"

TaskQueue findPathQueue;
TaskQueue moveCommandQueue;
pthread_mutex_t enqueueCommendMutex;

void initStaticValue () {
    initQueue(&findPathQueue);
    initQueue(&moveCommandQueue);
    pthread_mutex_init(&enqueueCommendMutex, NULL);
}

void destroyStaticValue() {
    pthread_mutex_destroy(&findPathQueue.mutex);
    pthread_mutex_destroy(&moveCommandQueue.mutex);
    pthread_mutex_destroy(&enqueueCommendMutex);
}

int main() {
    initStaticValue();

    FindPathTask* findPathTask = (FindPathTask*)malloc(sizeof(FindPathTask));
    findPathTask->tableNum = 14;
    
    enqueue(&findPathQueue, findPathTask);

    // 경로 계산 -> 작업 큐에 이동 커맨드 enqueue -> 작업 큐에 커맨드가 들어오기를 기다리는 이동 스레드가 이를 인식하여 이동

    // TODO : 복귀 버튼을 누르면 moveCommandQueue에 작업이 있는지 확인 후 작업이 있으면 작업실행, 아니면 복귀 작업을 큐에 넣는다.
    aStar();

    destroyStaticValue();
    return 0;
}