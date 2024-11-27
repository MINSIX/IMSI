#ifndef __ROBOT_MOVING_ENVENT_H__ // 헤더 가드 시작
#define __ROBOT_MOVING_ENVENT_H__ // 헤더 가드 정의

#include <pthread.h>

#define MAX_TASK_SIZE 100 // 큐의 최대 크기

#define DEFAULT_START_ROW 4
#define DEFAULT_START_COL 6

typedef struct TaskQueue{
    void* task[MAX_TASK_SIZE];
    int front;
    int rear;
    pthread_mutex_t mutex;
} TaskQueue;

typedef struct FindPathTask {
    int tableNum;
} FindPathTask;

extern TaskQueue findPathQueue;
extern TaskQueue moveCommandQueue;
extern pthread_mutex_t enqueueCommendMutex;

extern int nowRow;
extern int nowCol;

extern double roll;
extern double pitch;
extern double yaw;

extern void aStar();

extern void startMove();

extern void initQueue(TaskQueue* q);
extern int isEmpty(TaskQueue* q);
extern int isFull(TaskQueue* q);
extern void enqueue(TaskQueue* q, void* value);
extern void* dequeue(TaskQueue* q);
extern void* peek(TaskQueue* q);

#endif /* __ROBOT_MOVING_ENVENT_H__ */ // 헤더 가드 종료