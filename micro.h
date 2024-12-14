#ifndef MICRO_H
#define MICRO_H

#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define trigPin 27    //gpio 21
#define echoPin 22 
extern int soundmode;
extern int stopFlag;
void* distancecheck(void* arg);

#endif // MICRO_H