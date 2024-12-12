// #include "bluetooth.h"

// int main(void)
// {
//     int result = bluetoothGate(); // bluetoothInput 함수 호출
//     printf("\nResult: %d\n", result); // 결과 출력
//     return 0;
// }


// Sound.h 확인용
#include "sound.h"
#include "micro.h"
#include "bluetooth.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "robot_moving_event.h"

TaskQueue findPathQueue;
TaskQueue moveDestinationQueue;
pthread_mutex_t enqueueCommendMutex;

int soundmode = 1; // 음악 모드 (0: 멈춤, 1: 도착, 2: 위험, 3: 이동)
pthread_mutex_t modeMutex;

void initStaticValue () {
    initQueue(&findPathQueue);
    initQueue(&moveDestinationQueue);
    pthread_mutex_init(&enqueueCommendMutex, NULL);
}

void *musicThread(void *arg) {
    while (1) {
        pthread_mutex_lock(&modeMutex); // 뮤텍스 잠금
        int currentMode = soundmode;
        pthread_mutex_unlock(&modeMutex); // 뮤텍스 해제

        managerMusic(currentMode);
        usleep(100 * 1000); // 100ms 대기
    }
    return NULL;
}

void *bluetoothThread(void *arg) {
    printf("blutetooth thread \n");
    while (1) {
        int input = bluetoothGate();
        printf("enqueue: %d \n", input);
        FindPathTask* findPathTask = (FindPathTask*)malloc(sizeof(FindPathTask));
        findPathTask->tableNum = input;
        enqueue(&findPathQueue, findPathTask);
    }
    return NULL;
}


int main(int argc, char **argv) {
    // GPIO 초기화
    wiringPiSetupGpio();
    initStaticValue();
    // Mutex 초기화
    pthread_mutex_init(&modeMutex, NULL);
    
    int num_threads = 10;  // 생성할 스레드 개수
    pthread_t threads[num_threads];


    // 사운드 관리 스레드 시작

    if (pthread_create(&threads[0], NULL, bluetoothThread, NULL) != 0) {
        perror("블루투스 스레드 생성 실패");
        return -1;
    }
    if (pthread_create(&threads[1], NULL, musicThread, NULL) != 0) {
        perror("사운드 스레드 생성 실패");
        return -1;
    }
    if (pthread_create(&threads[2], NULL, aStar, NULL) != 0) {
        perror("Path 스레드 생성 실패");
        return -1;
    }
    // if (pthread_create(&threads[3], NULL, startMoveWheelThread, NULL) != 0) {
    //     perror("Move 스레드 생성 실패");
    //     return -1;
    // }

    
    for(int i = 0; i < num_threads; i++){
      if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join failed");
            exit(EXIT_FAILURE);
        }
    }
    pthread_mutex_destroy(&modeMutex);
    return 0;
}







    // printf("사운드 테스트 프로그램\n");
    // printf("모드를 선택하세요:\n");
    // printf("0: 음악 멈춤\n");
    // printf("1: 도착 음악 재생\n");
    // printf("2: 위험 음악 재생\n");
    // printf("3: 이동 음악 재생\n");
    // printf("Ctrl+C로 종료\n");

    // // 사용자 입력 루프
    // p

    // // 사용자 입력 루프
    // while (1) {
    //     int newMode;
    //     printf("모드 입력: ");
    //     scanf("%d", &newMode);

    //     if (newMode < 0 || newMode > 3) {
    //         printf("잘못된 모드 입력. 0~3 사이 값을 입력하세요.\n");
    //     } else {
    //         pthread_mutex_lock(&modeMutex); // 뮤텍스 잠금
    //         mode = newMode;
    //         pthread_mutex_unlock(&modeMutex); // 뮤텍스 해제
    //         printf("현재 모드: %d\n", mode);
    //     }
    // }