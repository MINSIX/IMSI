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

void change_marker2pos(int marker,int* row,int*col){
    int i = 0;
    int j = 0;

    int map[6][6] = {
        {0,100,2,3,4,5},
        {6,7,8,9,10,1},
        {12,13,14,15,16,17},
        {18,19,20,21,22,23},
        {24,25,26,27,28,29}
    };

    for (;i<6;i++){
        for (;j<6;j++){
            if (map[i][j] == marker){
                *row = i;
                *col = j;
                return;
            }
        }
    }
    return;
}

void* watch_and_read_file(){
    int fd,wd;
    FILE *file;
    //static long lastPosition = 0;
    char buffer[EVENT_BUF_LEN];

    char Smark[10];
    char flag[10];

    int marker = 0;

    int m_row=0;
    int m_col=0;

    MarkerRecognitionTask* markerRecognitionTask = (MarkerRecognitionTask*)malloc(sizeof(MarkerRecognitionTask));
    

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
                
                file  = fopen("test.txt","r");

                //fseek(file,lastPosition,SEEK_SET);

                char line[256];
                while (fgets(line,sizeof(line),file)){
                    splitString(line,Smark,flag);
                    printf("mark : %s\n",Smark);
                    printf("flag : %s\n",flag);

                    printf("marker : %s\n",Smark);

                    marker = atoi(Smark);
                    printf("marker : %d\n",marker);
                    change_marker2pos(marker,&m_row,&m_col);
                    
                    

                    markerRecognitionTask->row = m_row;
                    markerRecognitionTask->col = m_col;
                    enqueue(&markerRecognitionLogQueue, markerRecognitionTask);

                    printf("check row : %d %d\n",markerRecognitionTask->row,markerRecognitionTask->col);

                    if ( strcmp(flag,"None") == 32){
                        continue;
                    }
                    else if (strcmp(flag,"left") == 32){
                        leftFlag = 1;
                        rightFlag = 0;
                    }
                    else if (strcmp(flag,"right") == 32){
                        leftFlag = 0;
                        rightFlag = 1; 
                    }
                    else{
                        leftFlag = 0;
                        rightFlag = 0;
                    }
                    printf("le : %d ri: %d",leftFlag,rightFlag);

                }
                //lastPosition = ftell(file);
                fclose(file);
            }
            i += EVENT_SIZE + event->len;
        }
        
    }

    return NULL;

}
