#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <string.h>

#define BAUD_RATE 115200 //블루투스의 보율이 바뀔 경우 이 값을 변경해야함
static const char* UART2_DEV = ＂/dev/ttyAMA2＂; //RPi4: UART2 연결을 위한 장치 파일
//static const char* UART1_DEV = “/dev/ttyAMA1”; //RPi5: UART1 연결을 위한 장치 파일

unsigned char serialRead(const int fd);
void serialWrite(const int fd, const unsigned char c);
void serialWriteBytes (const int fd, const char *s);
//여러 바이트의 데이터를 씀

void serialWriteBytes (const int fd, const char *s)
{
    write (fd, s, strlen (s)) ;
}
//1바이트 데이터를 읽음
unsigned char serialRead(const int fd)
{
    unsigned char x;
        if(read (fd, &x, 1) != 1) //read 함수를 통해 1바이트 읽어옴
    return -1;
return x; //읽어온 데이터 반환
}
//1바이트 데이터를 씀
void serialWrite(const int fd, const unsigned char c)
{
    write (fd, &c, 1); //write 함수를 통해 1바이트 씀
}
int bluetoothInput ()
{
    int fd_serial ;
    unsigned char dat;
    char buf[100];
    if (wiringPiSetupGpio () < 0) return -1 ;
    if ((fd_serial = serialOpen (UART2_DEV, BAUD_RATE)) < 0) //UART2 포트 오픈
    {
        printf ("Unable to open serial device.\n") ;
        return -1;
    }
    while(serialDataAvail (fd_serial)){ //버퍼에 읽을 데이터가 있을 때까지 반복
        dat = serialRead (fd_serial); //버퍼에서 1바이트 값을 읽음
        printf ("%c", dat) ; //읽은 데이터 콘솔에 출력
        if(dat=='\n'){
            angle = atof(buf);
            break;
        }
        else {
            buff[i++] = dat;
        }
    }
    printf("\n");
    delay (10);
    return 0;
}