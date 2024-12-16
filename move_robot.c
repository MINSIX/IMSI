#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <wiringPi.h>
#include <time.h> 
#include "robot_moving_event.h"

int isReturn = 0;
extern int soundmode;


pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int commandReadyL = 0; // 명령이 준비되었는지 여부
int commandReadyR = 0; // 명령이 준비되었는지 여부
int stopFlag = 0;
int waitThreadCount = 0;
int goaldirection;
int goalRow = 0;
int goalCol = 0;
int goalDir = 0;
int markerNum = 0;

// int nowRobotRow;
// int nowRobotCol;
// int nowRobotDir;

#define RIGHT_PIN_COUNT 4
#define LEFT_PIN_COUNT 4

#define DEFAULT_DELAY_TIME 7
#define TURN_DELAY_TIME 3
#define PLUS_DELAY_TIME 1
#define MINUS_DELAY_TIME 3
#define PLUS_DEALY_WEIGHT 1

#define DELAY_TRANSFER_THRESHOLD_SEC 4

#define ANGLE_45 4100

int front = 0;
int leftFlag = 0;
int rightFlag = 0;
// 스텝 모터 핀 배열
// A(상) B(우) A'(하) B'(좌)
// 1, 2상 제어 : 4096 스텝
int right_arr[RIGHT_PIN_COUNT] = {20, 5, 12, 16};  // 오른쪽 모터 핀
int left_arr[LEFT_PIN_COUNT] = {26, 19, 13, 6};      // 왼쪽 모터 핀

MoveDestinationTask* clearQueue(TaskQueue* queue) {
    MoveDestinationTask* lastTask = NULL; // 마지막 Task를 저장할 포인터

    while (!isEmpty(queue)) {
        MoveDestinationTask* currentTask = (MoveDestinationTask*)dequeue(queue);
        if (lastTask != NULL) {
            free(lastTask); // 이전에 저장된 Task 메모리 해제
        }
        lastTask = currentTask; // 현재 Task를 마지막 Task로 저장
    }

    return lastTask; // 가장 마지막 Task를 반환
}



// 스텝 패턴
int one_phase[8][4] = {
    {1, 0, 0, 0},
    {1, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 1},
    {0, 0, 0, 1},
    {1, 0, 0, 1}
};

// 모터 초기화 함수
void init_Step(int* pin_arr) {
    for(int i = 0; i < 4; i++) {
        pinMode(pin_arr[i], OUTPUT);
    }
}

void moveFront(int* pin_arr, int delay_time, int stepNum) {
   
    pthread_mutex_lock(&distanceMutex);
    if(distanceStopFlag){
        pthread_cond_wait(&distanceCond, &distanceMutex);
    }
    pthread_mutex_unlock(&distanceMutex);
    for (int i = 0; i < 4; i++) {
        digitalWrite(pin_arr[i], one_phase[stepNum % 8][i]);
    }
    delay(delay_time);
}
void moveBack(int* pin_arr, int delay_time, int stepNum) {
    pthread_mutex_lock(&distanceMutex);

    if(distanceStopFlag){
        pthread_cond_wait(&distanceCond, &distanceMutex);
    }
    pthread_mutex_unlock(&distanceMutex);
    // printf("moveBack : %d\n", stepNum);
    for (int i = 0; i < 4; i++) {
        digitalWrite(pin_arr[i], one_phase[stepNum % 8][i]);
    }
    delay(delay_time);

}

void moveLeft(int* pin_arr, int isLeft, int delay_time, int stepNum) {
    if (isLeft) {
        // TODO : 실험을 통해 스텝 수 조절 (직각으로 도는데 걸리는 시간 or 왼쪽으로 도는데 걸리는 시간)
        for(int i = stepNum-1 ; i >= 0 ; i--) {
            moveBack(pin_arr, TURN_DELAY_TIME, i);
        }
    } else {
        // TODO : 실험을 통해 스텝 수 조절 (직각으로 도는데 걸리는 시간 or 왼쪽으로 도는데 걸리는 시간)
        for(int i = 0 ; i < stepNum ; i++) {
            moveFront(pin_arr, TURN_DELAY_TIME, i);
        }
    }
}
void moveRight(int* pin_arr, int isLeft, int delay_time, int stepNum) {
    if (isLeft) {
        // TODO : 실험을 통해 스텝 수 조절 (직각으로 도는데 걸리는 시간 or 오른쪽으로 도는데 걸리는 시간)
        for(int i = 0 ; i < stepNum ; i++) {
            moveFront(pin_arr, TURN_DELAY_TIME, i);
        }
    } else {
        // TODO : 실험을 통해 스텝 수 조절 (직각으로 도는데 걸리는 시간 or 오른쪽으로 도는데 걸리는 시간)
        for(int i = stepNum ; i > 0 ; i--) {
            moveBack(pin_arr, TURN_DELAY_TIME, i);
        }
    }
}


void moveWheel(int* pin_arr, int isLeft) {
    int delay_time = DEFAULT_DELAY_TIME;
    int leftFlagDuration = 0;
    int rightFlagDuration = 0;
    time_t leftFlagStartTime = 0;
    time_t rightFlagStartTime = 0;
    


    printf("nowRobotDir : %d, goalDir : %d\n", nowRobotDir, goalDir);
    

       
    if(nowRobotDir != goalDir) {
        goaldirection = (8+goalDir-nowRobotDir)%8;        
        printf("now direciton %d\n",goaldirection);
       for (int i = 0 ; i < 4500 ; i++) {
            // printf("front little : %d\n",i);
            moveFront(pin_arr, delay_time, i);
        }

        switch(goaldirection){
            case 1:
            moveRight(pin_arr, isLeft, delay_time, ANGLE_45);
            break;
            case 2:
            moveRight(pin_arr, isLeft, delay_time, ANGLE_45*2);
            break;
            case 3:
            moveRight(pin_arr, isLeft, delay_time, ANGLE_45*3);
            break;
            case 4:
            moveRight(pin_arr, isLeft, delay_time, ANGLE_45*4);
            break;
            
            case 7:
            moveLeft(pin_arr, isLeft, delay_time, ANGLE_45);
            break;
            case 6:
            moveLeft(pin_arr, isLeft, delay_time, ANGLE_45*2);
            break;
            case 5:
            moveLeft(pin_arr, isLeft, delay_time, ANGLE_45*3);
            break;

        }
    
    }
    printf("회전\n");
    while(1) {
        for(int i = 0; i < 256; i++) {
            // if(ultrasoundFlag) {
            // TODO : ultrasoundFlag가 true가 될 때까지 대기
            // }

            if(stopFlag) {
                printf("stop!\n");
                return;
            }

            if (leftFlag) {
                // if(leftFlag != -1) {
                    // 플래그가 활성화된 시간 기록
                    if (leftFlagStartTime == 0) {
                        leftFlagStartTime = time(NULL); 
                        // 플래그가 최초로 설정되면 임계시간을 넘지않아도 바로 회전을 할 수 있도록 설정
                        leftFlagDuration = DELAY_TRANSFER_THRESHOLD_SEC+1;
                    } else{
                        leftFlagDuration = time(NULL) - leftFlagStartTime;
                    }
                // }
            } else {
                // 플래그 비활성화 시 시작 시간 및 유지 시간 리셋
                leftFlagDuration = 0; 
                leftFlagStartTime = 0;
            }

            if (rightFlag) {
                // if(rightFlag != -1) {
                    // 플래그가 활성화된 시간 기록
                    if (rightFlagStartTime == 0) {
                        rightFlagStartTime = time(NULL);
                        // 플래그가 최초로 설정되면 임계시간을 넘지않아도 바로 회전을 할 수 있도록 설정
                        rightFlagDuration = DELAY_TRANSFER_THRESHOLD_SEC+1;
                    } else {
                        rightFlagDuration = time(NULL) - rightFlagStartTime; 
                    }
                // }
            } else {
                // 플래그 비활성화 시 시작 시간 및 유지 시간 리셋
                rightFlagDuration = 0; 
                rightFlagStartTime = 0;
            }

            if(isLeft) {
                if (rightFlag && rightFlagDuration > DELAY_TRANSFER_THRESHOLD_SEC) {
                    rightFlagStartTime = time(NULL); 
                    delay_time -= MINUS_DELAY_TIME;
                    if (delay_time <= 0) {
                        delay_time = 1;
                    }
                }
                if (leftFlag && leftFlagDuration > DELAY_TRANSFER_THRESHOLD_SEC) {
                    leftFlagStartTime = time(NULL); 
                    delay_time += PLUS_DELAY_TIME;
                }
            } else {
                if (rightFlag && (rightFlagDuration > DELAY_TRANSFER_THRESHOLD_SEC)) {
                    rightFlagStartTime = time(NULL); 
                    delay_time += PLUS_DELAY_TIME;
                }
                if (leftFlag && leftFlagDuration > DELAY_TRANSFER_THRESHOLD_SEC) {
                    leftFlagStartTime = time(NULL); 
                    delay_time -= MINUS_DELAY_TIME;
                    if (delay_time <= 0) {
                        delay_time = 1;
                    }
                }
            }

            // 플래그가 모두 0일 때 기본 지연 시간으로 설정
            // if (!leftFlag && !rightFlag) {
            //     delay_time = DEFAULT_DELAY_TIME; 
            // }
            
            // 직각일 상황에서 멈추고 도는게 아니라면 임계치를 넘었을 때 뒤로 돌게 해야함
            // if (delay_time > DEFAULT_DELAY_TIME + ) {
            //     moveBack(pin_arr, -(delay_time), i);
            // } else {
            //     moveFront(pin_arr, delay_time, i);
            // }
            if(front) {
                // printf("leftFlag, rightFlag is -1\n");
                // printf("delay_time : %d\n", delay_time);
                // printf("front -> isLeft : %d, delay_time : %d\n", isLeft, delay_time);
                
                moveFront(pin_arr, delay_time*8, i);
            } else {
                // printf("isLeft : %d, delay_time : %d\n", isLeft, delay_time);
                moveFront(pin_arr, delay_time, i);
            }
        }
    }
    
    printf("직진\n");
}

void* leftWheelThread(void* arg) {
    printf("leftWheel Thread\n");
    while (1) {
        pthread_mutex_lock(&mutex); // 뮤텍스 잠금
        while (!commandReadyL) {
            waitThreadCount++;
            pthread_cond_wait(&cond, &mutex);

        }
        pthread_mutex_unlock(&mutex); // 뮤텍스 해제
        waitThreadCount = 0;
               
        printf("leftwheel\n");
        moveWheel(left_arr, 1);
        delay(10);
        printf("left end!\n");
    }
    return NULL;
}

void* rightWheelThread(void* arg) {
    printf("rightWheel Thread\n");
   
    while (1) {
        pthread_mutex_lock(&mutex); // 뮤텍스 잠금
        while (!commandReadyR) {
            waitThreadCount++;
            pthread_cond_wait(&cond, &mutex);
        }
        pthread_mutex_unlock(&mutex); // 뮤텍스 해제
        waitThreadCount = 0;
               
        printf("rightwheel\n");
        moveWheel(right_arr, 0);
        delay(10);
        printf("end!\n");
    }
}

void* startMoveWheelThread(void* arg) {
    printf("start move\n");
    wiringPiSetupGpio(); // WiringPi 초기화
    init_Step(right_arr);
    init_Step(left_arr);

    pthread_t leftThread, rightThread;

    pthread_create(&leftThread, NULL, leftWheelThread, NULL);
    pthread_create(&rightThread, NULL, rightWheelThread, NULL);
    // 바퀴가 동시에 돌기 위해서 둘 다 wait 상태에 걸리도록 조금 대기해야함!!
    delay(10);

    while(1) {
        MoveDestinationTask* task = dequeue(&moveDestinationQueue);
        printf("dequeue direction\n");
        delay(10);

        goalRow = task->row;
        goalCol = task->col;
        goalDir = task->direction;
        markerNum = task->markerNum;
        stopFlag = 0;
        pthread_mutex_lock(&mutex); // 뮤텍스 잠금
        commandReadyL = 1; // 명령 준비 완료
        commandReadyR = 1; // 명령 준비 완료
        pthread_mutex_unlock(&mutex);
        while (1) {
                pthread_cond_broadcast(&cond); // 모든 스레드에 신호 전송
                
                break;
            
        }
        
        // 실험을 위한 코드(15초 동안 진행)
        // delay(30000);
        // commandReady = 0;
        // stopFlag = 1;

        // 마커인식쪽에서 마커를 인식하면 인식된 마커 번호를 전달하게 하여, 목표 위치와 일치하는지 확인 및 동작 중지
        while(1) {
            MarkerRecognitionTask* marker = dequeue(&markerRecognitionLogQueue);
            nowRobotRow = marker->row;
            nowRobotCol = marker->col;
            printf("nowRobotRow : %d, nowRobotCol : %d goalRow : %d goalCol : %d\n", nowRobotRow, nowRobotCol,goalRow,goalCol);

        
            if (goalRow == nowRobotRow && goalCol == nowRobotCol) {
                printf("right marker!\n");
                nowRobotDir = goalDir;
                // while(commandReadyL||commandReadyR){
                commandReadyL = 0;
                commandReadyR = 0;
                // }
                pthread_cond_broadcast(&cond); // 모든 스레드에 신호 전송
                commandReadyL = 0;
                commandReadyR = 0;
                stopFlag = 1;
                break;
            } else {
                // 잘못된 위치에서 새로운 경로 탐색
        // FindPathTask* newPathTask = (FindPathTask*)malloc(sizeof(FindPathTask));
        
        // newPathTask->tableNum = clearQueue(&moveDestinationQueue)->markerNum; // 새로운 목표 설정 (필요에 따라 수정)
        // printf("%d\n",newPathTask->tableNum);
        // aStar(newPathTask);        // 경로 탐색 호출
        
        // printf("새 경로를 따라 이동을 시작합니다.\n");
            printf("잘못된경로 \n");
            // 새로운 경로를 따라 이동
            return NULL;
        break; // 루프 탈출, 새로운 경로로 재시작
            }
        }
        delay(20);
        
        // 최종 목표에 도착하였다면 복귀로직 시작
        if (isEmpty(&moveDestinationQueue)) {
            
            isReturn = isReturn ^ 1;
            if(isReturn) {
                printf("return!\n");
                soundmode = 1;

                // 로봇 복귀를 위한 경로 탐색 후 동작
                FindPathTask* findPathTask = (FindPathTask*)malloc(sizeof(FindPathTask));
                findPathTask->tableNum = 1;
                aStar(findPathTask);
                // 10초 대기 후 복귀 동작
                delay(10000);
                soundmode = 3;
            }
        }
        // TODO : 작업에 대한 복귀가 끝나서 다음 작업을 들여보내라는 신호를 줘야함
    }
    printf("end move\n");
    return NULL;
}
