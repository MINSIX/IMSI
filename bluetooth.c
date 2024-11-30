#include "bluetooth.h"

unsigned char serialRead(const int fd)
{
    unsigned char x;
    if (read(fd, &x, 1) != 1) // 1바이트 읽기
        return -1;
    return x; // 읽은 데이터 반환
}

void serialWrite(const int fd, const unsigned char c)
{
    write(fd, &c, 1); // 1바이트 쓰기
}

void serialWriteBytes(const int fd, const char *s)
{
    write(fd, s, strlen(s)); // 여러 바이트 데이터 쓰기
}

int bluetoothInput(void)
{
    int fd_serial;
    unsigned char dat;
    char buf[100] = {0};
    int return_;
    int i = 0;

    if (wiringPiSetupGpio() < 0) {
        printf("WiringPi setup failed.\n");
        return -1;
    }

    if ((fd_serial = serialOpen(UART2_DEV, BAUD_RATE)) < 0) {
        printf("Unable to open serial device.\n");
        return -1;
    }

    // 시리얼 데이터가 올 때까지 대기
    while (!serialDataAvail(fd_serial)) {
        usleep(1000); // CPU 과부하를 방지하기 위해 잠시 대기
    }

    while (serialDataAvail(fd_serial)) {
        dat = serialRead(fd_serial); // 1바이트 데이터 읽기
        //printf("%c", dat); // 읽은 데이터 콘솔에 출력
        if (dat == '\n') {
            buf[i] = '\0'; // 버퍼 종료
            return_ = atoi(buf); // 버퍼의 숫자 문자열을 정수로 변환
            break;
        } else {
            buf[i++] = dat; // 버퍼에 데이터 저장
        }
    }

    //printf("[1] %d [2]", return_);
    return return_; // 읽은 정수 값을 반환
}

//정수가 아닌경우의 처리를 위한 게이트
int bluetoothGate(void){
    int input = 0;
    do{
        input = bluetoothInput();
    }while(input == 0);
    return input;
}