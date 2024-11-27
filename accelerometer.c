#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <math.h>
#include "robot_moving_event.h"

#define CS_GPIO 8 // CS
#define SPI_CH 0
#define SPI_SPEED 1000000 // 1MHz
#define SPI_MODE 3

#define BW_RATE 0x2C // 통신속도 설정
#define POWER_CTL 0x2D // Power Control Register(measure를 1로 바꿔줘야하는 레지스터)

#define DATA_FORMAT 0x31 // gRange 설정 레지스터
#define DATAX0 0x32 // X-Axis Data 0
#define DATAX1 0x33 // X-Axis Data 1
#define DATAY0 0x34 // Y-Axis Data 0
#define DATAY1 0x35 // Y-Axis Data 1
#define DATAZ0 0x36 // Z-Axis Data 0
#define DATAZ1 0x37 // Z-Axis Data 1

//ADXL345
void readRegister_ADXL345(char registerAddress, int numBytes, char * values){
    //read 1 .
    values[0] = 0x80 | registerAddress;
    // 6 1
    if(numBytes > 1) values[0] = values[0] | 0x40;
    digitalWrite(CS_GPIO, LOW); // Low : CS Active
    // values
    wiringPiSPIDataRW(SPI_CH, values, numBytes + 1);
    digitalWrite(CS_GPIO, HIGH); // High : CS Inactive
}

//ADXL345
void writeRegister_ADXL345(char address, char value) {
    unsigned char buff[2];
    buff[0] = address;
    buff[1] = value;
    digitalWrite(CS_GPIO, LOW); // Low : CS Active
    wiringPiSPIDataRW(SPI_CH, buff, 2);
    digitalWrite(CS_GPIO, HIGH); // High : CS Inactive
}

int main (void) {
    unsigned char buffer[100];
    short x, y= 0 , z= 0;
    if(wiringPiSetupGpio() == -1) return 1;
    if(wiringPiSPISetupMode(SPI_CH, SPI_SPEED,SPI_MODE) == -1) return 1;
    pinMode(CS_GPIO, OUTPUT); //Chip Select OUTPUT

    digitalWrite(CS_GPIO, HIGH); //IDLE
    writeRegister_ADXL345(DATA_FORMAT, 0x09); // +- 4G + full resolution
    writeRegister_ADXL345(BW_RATE,0x0C); // Output Data Rate 400 Hz
    writeRegister_ADXL345(POWER_CTL,0x08); // measure 1로 변경
    
    float translateNum = 0.0039; // full resolution로 변경한 데이터셋에 따라 곱해줄 값

    while(1) {
        readRegister_ADXL345(DATAX0,6,buffer); //
        x = ((short)buffer[2]<<8)|(short)buffer[1]; //X
        y = ((short)buffer[4]<<8)|(short)buffer[3]; //Y
        z = ((short)buffer[6]<<8)|(short)buffer[5]; //Z
        roll = atan2(y, sqrt(x*x + z*z)) * (180 / M_PI);
        pitch = atan2(x, sqrt(y*y + z*z)) * (180 / M_PI);
        yaw = atan2(y, x) * (180 / M_PI);
        // printf("x : %f, y: %f, z : %f, roll : %f, pitch : %f\n", x * translateNum, y * translateNum, z * translateNum, roll, pitch); //X, Y, Z
        
        // TODO : 사고 방지 
        if (roll > ? || roll < ? || pitch > ? || pitch < ?) {
            // 위험 부저 울리기
        }
        
        delay(50);
    }

    return 0;
}