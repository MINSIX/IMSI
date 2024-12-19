#ifndef MICRO_H
#define MICRO_H

#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define trigPin 27    //gpio 21
#define echoPin 22 

extern pthread_mutex_t modeMutex;
extern int distanceStopFlag;
extern pthread_cond_t distanceCond;
extern pthread_mutex_t distanceMutex;
extern int soundmode;

    
void* distancecheck(void* arg);

#endif // MICRO_H