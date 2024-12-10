#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <wiringPi.h>
#include <wiringSerial.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BAUD_RATE 115200 // 블루투스의 보율이 바뀔 경우 이 값을 변경해야 함
static const char* UART1_DEV = "/dev/ttyAMA1"; //RPi5: UART1 연결을 위한 장치 파일

unsigned char serialRead(const int fd);
void serialWrite(const int fd, const unsigned char c);
void serialWriteBytes(const int fd, const char *s);
int bluetoothInput(void); // bluetoothInput 함수 선언 (반환형 int)
int bluetoothGate(void);
#endif // BLUETOOTH_H