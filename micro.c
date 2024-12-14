#include "micro.h"
   //gpio 18
 
//old #define trigPin 21    //gpio 5
//old #define echoPin 4    //gpio  J16-pin3 GPIO 23
 
void* distancecheck(void* arg)
{
    int distance=0;
    int pulse = 0;
    int turnOn = 0;
    long startTime;
    long travelTime;

        
    pinMode (trigPin, OUTPUT);
    pinMode (echoPin, INPUT);
    
    while(1)
    {
        digitalWrite (trigPin, LOW);
        usleep(2);
        digitalWrite (trigPin, HIGH);
        usleep(20);
        digitalWrite (trigPin, LOW);
        
        while(digitalRead(echoPin) == LOW);
        
        startTime = micros();
        
        while(digitalRead(echoPin) == HIGH);
        travelTime = micros() - startTime;
        
        int distance = travelTime / 58;
        
        //printf( "\nDistance: %dcm\n", distance);
        delay(200);
        if(distance < 10) {
          soundmode = 2;
          turnOn = 1;
          stopFlag = 1;
        }
        else if(turnOn && distance >= 10){
          soundmode = 3;
          turnOn = 0;
          stopFlag = 0;
        }
    }
}
 