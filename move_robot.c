#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <wiringPi.h>
#include <time.h> 
#include "robot_moving_event.h"

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int commandReady = 0; // 명령이 준비되었는지 여부
int stopFlag = 0;
int waitThreadCount = 0;

#define RIGHT_PIN_COUNT 4
#define LEFT_PIN_COUNT 4
#define DEFAULT_DELAY_TIME 3
#define THRESHOLD_SEC 2

// 스텝 모터 핀 배열
// A(상) B(우) A'(하) B'(좌)
// 1, 2상 제어 : 4096 스텝
int right_arr[RIGHT_PIN_COUNT] = {20, 5, 12, 16};  // 오른쪽 모터 핀
int left_arr[LEFT_PIN_COUNT] = {26, 19, 13, 6};      // 왼쪽 모터 핀

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

void moveFront(int* pin_arr, int isLeft) {
    int delay_time = DEFAULT_DELAY_TIME;
    int leftFlagDuration = 0;
    int rightFlagDuration = 0;
    time_t leftFlagStartTime = 0;
    time_t rightFlagStartTime = 0;
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
                // 플래그가 활성화된 시간 기록
                if (leftFlagStartTime == 0) {
                    leftFlagStartTime = time(NULL); 
                    // 플래그가 최초로 설정되면 임계시간을 넘지않아도 바로 회전을 할 수 있도록 설정
                    leftFlagDuration = THRESHOLD_SEC+1;
                } else{
                    leftFlagDuration = time(NULL) - leftFlagStartTime;
                }
            } else {
                // 플래그 비활성화 시 시작 시간 및 유지 시간 리셋
                leftFlagDuration = 0; 
                leftFlagStartTime = 0;
            }

            if (rightFlag) {
                // 플래그가 활성화된 시간 기록
                if (rightFlagStartTime == 0) {
                    rightFlagStartTime = time(NULL);
                    // 플래그가 최초로 설정되면 임계시간을 넘지않아도 바로 회전을 할 수 있도록 설정
                    rightFlagDuration = THRESHOLD_SEC+1;
                } else {
                    rightFlagDuration = time(NULL) - rightFlagStartTime; 
                }
                
            } else {
                // 플래그 비활성화 시 시작 시간 및 유지 시간 리셋
                rightFlagDuration = 0; 
                rightFlagStartTime = 0;
            }


            if(isLeft) {
                if (rightFlag && rightFlagDuration > THRESHOLD_SEC) {
                    rightFlagStartTime = time(NULL); 
                    delay_time--;
                    if (delay_time <= 0) {
                        delay_time = 1;
                    }
                }
                if (leftFlag && leftFlagDuration > THRESHOLD_SEC) {
                    leftFlagStartTime = time(NULL); 
                    delay_time++;
                }
            } else {
                if (rightFlag && (rightFlagDuration > THRESHOLD_SEC)) {
                    rightFlagStartTime = time(NULL); 
                    delay_time++;
                }
                if (leftFlag && leftFlagDuration > THRESHOLD_SEC) {
                    leftFlagStartTime = time(NULL); 
                    delay_time--;
                    if (delay_time <= 0) {
                        delay_time = 1;
                    }
                }
            }

            // 플래그가 모두 0일 때 기본 지연 시간으로 설정
            if (!leftFlag && !rightFlag) {
                delay_time = DEFAULT_DELAY_TIME; 
            }

            for (int j = 0; j < 4; j++) {
                digitalWrite(pin_arr[j], one_phase[i % 8][j]);
            }
            delay(delay_time);
        }
    }
}
void moveBack(int* pin_arr, int isLeft) {
    int delay_time = DEFAULT_DELAY_TIME;
    while(1) {
        for(int i = 4096; i > 0; i--) {
            // if(ultrasoundFlag) {
            //     // TODO : ultrasoundFlag가 true가 될 때까지 대기
            // }
            if(stopFlag) {
                return;
            }

            if(isLeft) {
                // if (rightFlag) {
                //     delay_time++;
                // }
                // if (leftFlag) {
                //     delay_time--;
                //     if (delay_time <= 0) {
                //         delay_time = 1;
                //     }
                // }
            } else {
                // if (rightFlag) {
                //     delay_time--;
                //     if (delay_time <= 0) {
                //         delay_time = 1;
                //     }
                // }
                // if (leftFlag) {
                //     delay_time++;
                // }
            }

            // 플래그가 모두 0일 때 기본 지연 시간으로 설정
            // if (!leftFlag && !rightFlag) {
            //     delay_time = DEFAULT_DELAY_TIME; 
            // }

            for (int j = 0; j < 4; j++) {
                digitalWrite(pin_arr[j], one_phase[i % 8][j]);
            }
            delay(delay_time);
        }
    }
}

void* leftWheelThread(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex); // 뮤텍스 잠금
        while (!commandReady) {
            waitThreadCount++;
            pthread_cond_wait(&cond, &mutex);
        }
        pthread_mutex_unlock(&mutex); // 뮤텍스 해제

        moveFront(left_arr, 1);
    }
    return NULL;
}

void* rightWheelThread(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex); // 뮤텍스 잠금
        while (!commandReady) {
            waitThreadCount++;
            pthread_cond_wait(&cond, &mutex);
        }
        pthread_mutex_unlock(&mutex); // 뮤텍스 해제

        moveFront(right_arr, 0);
        // moveBack(right_arr, 0);
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
        if (isEmpty(&moveDestinationQueue)) {
            continue;
        }
        
        delay(10);
        printf("dequeue direction\n");
        MoveDestinationTask* task = dequeue(&moveDestinationQueue);
        commandReady = 1; // 명령 준비 완료
        while (1) {
            if (waitThreadCount == 2) {
                waitThreadCount = 0;
                pthread_cond_broadcast(&cond); // 모든 스레드에 신호 전송
                break;
            }
        }
        
        // 실험을 위한 코드(15초 동안 진행)
        // delay(15000);
        // commandReady = 0;
        // stopFlag = 1;

        // 마커인식쪽에서 마커를 인식하면 인식된 마커 번호를 전달하게 하여, 목표 위치와 일치하는지 확인 및 동작 중지
        int goalRow = task->row;
        int goalCol = task->col;
        while(1) {
            if (isEmpty(&markerRecognitionLogQueue)) {
                continue;
            }
            MarkerRecognitionTask* marker = dequeue(&markerRecognitionLogQueue);
            if (goalRow == marker->row && goalCol == marker->col) {
                commandReady = 0;
                stopFlag = 1;
                break;
            } else {
                printf("잘못된 위치입니다!\n");
                return;
                // TODO : 잘못된 위치라면 큐에 있는 경로 모두 삭제 후 다시 경로 계산?
            }
        }

        // TODO : 버튼 클릭을 기다리고, 클릭되면 되돌아가기 함수 넣어야함

        // 1. 큐에 작업이 있으면 dequeue 후 작업에 있는 목표 row, col까지 라인트레이싱하면서 진행
        // 2. 진행하면서 라인트레이싱 스레드가 지시히는 leftFlag, rightFlag를 통해 바퀴 속도 제어
        // 3. 진행 중 센싱 (초음파 센서를 통해 장애물이 앞에 있으면 멈추고 부저를 울린다. / 가속도 센서의 각이 갑자기 엇나가는 순간에 부저를 울린다.)
        // 4. 진행 중 마커 인식 스레드가 마커를 인식하고 마커를 통한 현재위치 정보(row, col)를 전달하면, 목표 row, col과 대조
        // 5. 위의 과정을 반복하여 목표 위치에 도착 후 복귀 명령이 올 때 까지 무한 대기
        // 6. 복귀는 현재 위치까지 올 때 기록해놨던 스택에서 pop하면서 진행(단, 후진을 기본값으로 설정)
    }
    printf("end move\n");
}