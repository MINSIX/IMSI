#include <stdio.h>
#include <stdlib.h>
#include "robot_moving_event.h"
#include "readCameraInfo.h"

TaskQueue findPathQueue;
TaskQueue moveDestinationQueue;
TaskQueue markerRecognitionLogQueue;

pthread_mutex_t enqueueCommendMutex;

void initStaticValue () {
    initQueue(&findPathQueue);
    initQueue(&moveDestinationQueue);
    initQueue(&markerRecognitionLogQueue);
    pthread_mutex_init(&enqueueCommendMutex, NULL);
}

void destroyStaticValue() {
    pthread_mutex_destroy(&findPathQueue.mutex);
    pthread_mutex_destroy(&moveDestinationQueue.mutex);
    pthread_mutex_destroy(&markerRecognitionLogQueue.mutex);
    pthread_mutex_destroy(&enqueueCommendMutex);
}

// 로봇 전진 테스트 main
int main() {
    initStaticValue();

    MoveDestinationTask* moveDestinationTask = (MoveDestinationTask*)malloc(sizeof(MoveDestinationTask));
    moveDestinationTask->row = 1;
    moveDestinationTask->col = 5;
    
    enqueue(&moveDestinationQueue, moveDestinationTask);

    
    
    // MarkerRecognitionTask* markerRecognitionTask = (MarkerRecognitionTask*)malloc(sizeof(MarkerRecognitionTask));
    // markerRecognitionTask->row = 0;
    // markerRecognitionTask->col = 0;
    // enqueue(&markerRecognitionLogQueue, markerRecognitionTask);
    

    pthread_t moveWheelThread;
    pthread_create(&moveWheelThread, NULL, startMoveWheelThread, NULL);
    pthread_create(&moveWheelThread, NULL, watch_and_read_file,NULL);
    
    // flag change and marker dlstlr
    pthread_join(moveWheelThread, NULL); 

    destroyStaticValue();
    return 0;
}