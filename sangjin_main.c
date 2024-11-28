// #include "bluetooth.h"

// int main(void)
// {
//     int result = bluetoothGate(); // bluetoothInput 함수 호출
//     printf("\nResult: %d\n", result); // 결과 출력
//     return 0;
// }


// Sound.h 확인용
#include "sound.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int mode = 0; // 음악 모드 (0: 멈춤, 1: 도착, 2: 위험, 3: 이동)

// managerMusic을 스레드에서 실행
#include <pthread.h>
pthread_mutex_t modeMutex;

void *musicThread(void *arg) {
    while (1) {
        pthread_mutex_lock(&modeMutex); // 뮤텍스 잠금
        int currentMode = mode;
        pthread_mutex_unlock(&modeMutex); // 뮤텍스 해제

        managerMusic(currentMode);
        usleep(100 * 1000); // 100ms 대기
    }
    return NULL;
}
int main(int argc, char **argv) {
    pthread_t threadId;

    // GPIO 초기화
    wiringPiSetupGpio();

    // 사운드 관리 스레드 시작
    if (pthread_create(&threadId, NULL, musicThread, NULL) != 0) {
        perror("스레드 생성 실패");
        return -1;
    }

    printf("사운드 테스트 프로그램\n");
    printf("모드를 선택하세요:\n");
    printf("0: 음악 멈춤\n");
    printf("1: 도착 음악 재생\n");
    printf("2: 위험 음악 재생\n");
    printf("3: 이동 음악 재생\n");
    printf("Ctrl+C로 종료\n");

    // 사용자 입력 루프
    pthread_mutex_init(&modeMutex, NULL);

    // 사용자 입력 루프
    while (1) {
        int newMode;
        printf("모드 입력: ");
        scanf("%d", &newMode);

        if (newMode < 0 || newMode > 3) {
            printf("잘못된 모드 입력. 0~3 사이 값을 입력하세요.\n");
        } else {
            pthread_mutex_lock(&modeMutex); // 뮤텍스 잠금
            mode = newMode;
            pthread_mutex_unlock(&modeMutex); // 뮤텍스 해제
            printf("현재 모드: %d\n", mode);
        }
    }

    pthread_join(threadId, NULL);
    pthread_mutex_destroy(&modeMutex);
    return 0;
}