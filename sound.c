#include "sound.h"


void arrivalMusicPlay()
{
    int i;
    softToneCreate (GPIO);

    for(i = 0 ; i < ARRIVAL_NOTES; i++)
    {
        softToneWrite (GPIO, arrivalNotes[i]); //
        delay (250); //
    }
}

void dangerMusicPlay()
{
    int i;
    softToneCreate (GPIO);

    for(i = 0 ; i < DANGER_NOTES; i++)
    {
        softToneWrite (GPIO, dangerNotes[i]); //
        delay (dangerDurations[i]); //
    }
}

void moveMusicPlay(){

    int i;
    softToneCreate (GPIO);
    
    for(i = 0 ; i < MOVE_NOTES; i++)
    {
        softToneWrite (GPIO, moveNotes[i]); //
        delay (moveNoteDuration[i]); //
    }

}

void stopMusic(){
    softToneWrite (GPIO, 0); //
}

void managerMusic(int mode) {
    switch (mode) {
        case 0: // 음악 멈춤
            stopMusic();
            break;

        case 1: // 도착 음악 재생
            arrivalMusicPlay();
            break;

        case 2: // 위험 음악 재생
            dangerMusicPlay();
            break;

        case 3: // 이동 음악 재생
            moveMusicPlay();
            break;

        default:
            stopMusic();
            break;
    }
}
