#include "micro.h"
   //gpio 18
 
//old #define trigPin 21    //gpio 5
//old #define echoPin 4    //gpio  J16-pin3 GPIO 23
int distanceStopFlag = 0;
pthread_cond_t distanceCond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t distanceMutex;
void* distancecheck(void* arg)
{
    wiringPiSetupGpio();
    pthread_mutex_init(&distanceMutex,NULL);
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
        int presound;
        int distance = travelTime / 58;
        delay(200);
        if(distance < 25) {
          turnOn = 1;
          printf("%d \n",distance);
          if(soundmode != 2){
            presound = soundmode;
          }
          
          soundmode = 2;

          pthread_mutex_lock(&distanceMutex);
          distanceStopFlag = 1;
          pthread_mutex_unlock(&distanceMutex);
        }
        else if(turnOn && distance >= 25){
          soundmode = presound;
          pthread_mutex_lock(&distanceMutex);
          pthread_cond_broadcast(&distanceCond);

          distanceStopFlag = 0;
          pthread_mutex_unlock(&distanceMutex);
          
        }

    }
}
 