#include "micro.h"

void* distancecheck(void* arg)
{
  printf("\nhi distance\n");
  int trig = 27 ;
  int echo = 22 ;
  int start_time, end_time ;
  float distance ;
  if (wiringPiSetup() == -1) exit(1) ;

  pinMode(trig, OUTPUT) ;
  pinMode(echo , INPUT) ;
  int threshold = 30;


  while(1) {
    digitalWrite(trig, LOW) ;
    delay(500) ;
    digitalWrite(trig, HIGH) ;
    delayMicroseconds(10) ;
    digitalWrite(trig, LOW) ;
    while (digitalRead(echo) == 0) ;
    start_time = micros() ;
    while (digitalRead(echo) == 1) ;
    end_time = micros() ;
    distance = (end_time - start_time) / 29. / 2. ;
    printf("distance %.2f cm\n", distance) ;
    if(distance < threshold)
    {
      soundmode = 3;
        //뮤텍스 중단
      printf("distance is lower than threshold");
    }
    else{
      printf("distance is higher than threshold");
    }


  }

  return NULL;

}

