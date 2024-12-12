#ifndef __READ_CAMERA_INFO__ // 헤더 가드 시작
#define __READ_CAMERA_INFO__ // 헤더 가드 정의
#include <sys/inotify.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#define EVENT_SIZE (sizeof(struct inotify_event))
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))

//const char* cameraFilePath= "test.txt";


void* watch_and_read_file();
void splitString(const char *str, char *numbers,char *nonNumbers);
#endif