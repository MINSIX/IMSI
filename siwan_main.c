#include <stdio.h>
#include <stdlib.h>
#include "robot_moving_event.h"

TaskQueue findPathQueue;
TaskQueue moveDestinationQueue;
pthread_mutex_t enqueueCommendMutex;

void initStaticValue () {
    initQueue(&findPathQueue);
    initQueue(&moveDestinationQueue);
    pthread_mutex_init(&enqueueCommendMutex, NULL);
}

void destroyStaticValue() {
    pthread_mutex_destroy(&findPathQueue.mutex);
    pthread_mutex_destroy(&moveDestinationQueue.mutex);
    pthread_mutex_destroy(&enqueueCommendMutex);
}

// 최단 경로 탐색 테스트 main
int main() {
    initStaticValue();

    FindPathTask* findPathTask = (FindPathTask*)malloc(sizeof(FindPathTask));
    findPathTask->tableNum = -3;
    enqueue(&findPathQueue, findPathTask);

    pthread_t aStarThread;
    pthread_create(&aStarThread, NULL, aStar, NULL);
    
    pthread_join(aStarThread, NULL); 

    destroyStaticValue();
    return 0;
}

// 로봇 전진 테스트 main
// int main() {
//     initStaticValue();

//     MoveDestinationTask* moveDestinationTask = (MoveDestinationTask*)malloc(sizeof(MoveDestinationTask));
//     moveDestinationTask->row = 14;
//     moveDestinationTask->col = 3;
//     enqueue(&moveDestinationQueue, moveDestinationTask);

//     pthread_t moveWheelThread;
//     pthread_create(&moveWheelThread, NULL, startMoveWheelThread, NULL);
    
//     pthread_join(moveWheelThread, NULL); 

//     destroyStaticValue();
//     return 0;
// }