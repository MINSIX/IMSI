#ifndef SOUND_H
#define SOUND_H

#include <wiringPi.h>
#include <softTone.h>
#include <stdio.h>
#include <stdlib.h>

#define ARRIVAL_NOTES 10
#define MOVE_NOTES 24
#define DANGER_NOTES 12
#define GPIO 18

extern int soundmode;

static int arrivalNotes[ARRIVAL_NOTES] = {
391,415,440,391,415,440,391,415,440,523
};


static int moveNotes[MOVE_NOTES] = {
     262, 294, 330, 392, 440, 392, 349, 330, // 첫 번째 구간
    294, 262, 294, 330, 349, 392, 330, 294, // 두 번째 구간
    440, 392, 349, 330, 349, 330, 294, 262
};

static int moveNoteDuration[] = {
    300, 300, 300, 400, 400, 300, 300, 300, // 첫 번째 구간
    300, 300, 300, 400, 300, 400, 300, 300, // 두 번째 구간
    400, 300, 300, 300, 300, 300, 300, 500  // 세 번째 구간
};

static int dangerNotes[DANGER_NOTES] = {
784, 880, 784, 659, 0, 784, 659, 784, 880, 784, 659, 0
};

static int dangerDurations[] = {
    200, 200, 200, 200, 150, 200, 200, 200, 200, 200, 200, 150
};

void arrivalMusicPlay(); // 도착했을 때 나는 소리
void moveMusicPlay();
void stopMusic();
void dangerMusicPlay();
void managerMusic(int mode);

#endif 