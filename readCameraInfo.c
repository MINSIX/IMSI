#include "readCameraInfo.h"
#include "robot_moving_event.h"

void splitString(const char *str, char *numbers,char *nonNumbers){
    int numIndex =0;
    int nonNumIndex =0;
    int len = strlen(str);
    for ( int i = 0; i<len;i++){
        if(isdigit(str[i])){
            numbers[numIndex++] = str[i]; 
        }
        else{
            nonNumbers[nonNumIndex++] = str[i];
        }
    }

    numbers[numIndex] = '\0';
    nonNumbers[nonNumIndex]='\0';
}

void* watch_and_read_file(){
    int fd,wd;
    char buffer[EVENT_BUF_LEN];

    char Smark[10];
    char flag[10];

    fd = inotify_init();
    if (fd < 0){
        perror("init");
        exit(EXIT_FAILURE);
    }

    wd = inotify_add_watch(fd, "test.txt",IN_MODIFY);
    
    printf("start\n");
    
    while(1){
        int length = read(fd,buffer,EVENT_BUF_LEN);

        int i =0;
        while(i<length){
            struct inotify_event *event = (struct inotify_event*)&buffer[i];
            if (event->mask & IN_MODIFY){
                
                FILE *file  = fopen("test.txt","r");
             
                char line[256];
                while (fgets(line,sizeof(line),file)){
                    splitString(line,Smark,flag);
                    printf("mark : %s\n",Smark);
                    printf("flag : %s\n",flag);

                    if (strcmp(flag,"left")){
                        leftFlag = 1;
                        rightFlag = 0;
                    }
                    else if (strcmp(flag,"right")){
                        leftFlag = 0;
                        rightFlag = 1; 
                    }
                    else{
                        leftFlag = 0;
                        rightFlag = 1;
                    }

                }
                fclose(file);
            }
            i += EVENT_SIZE + event->len;
        }
        
    }

    return NULL;

}
