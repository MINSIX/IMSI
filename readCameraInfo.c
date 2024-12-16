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

    int map[8][6] = {
        {0,100,1,6,0,0},
        {0,0,8,9,10,2},
        {0,13,14,15,16,17},
        {4,19,20,5,22,23},
        {24,25,26,27,28,29},
        {0,25,26,27,28,29},
        {3,25,26,7,28,29}
    };

    for (i = 0;i<8;i++){
        for (j = 0;j<6;j++){
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

    int m_tmp[10]={0,};
    int m_idx = 0;
    int m_flag = 0;

    int m_row=0;
    int m_col=0;

    int return_flag_tmp = 0;

    int check_row = 0;
    int check_col = 0;
    MarkerRecognitionTask* markerRecognitionTask = (MarkerRecognitionTask*)malloc(sizeof(MarkerRecognitionTask));
    

    fd = inotify_init();
    if (fd < 0){
        perror("init");
        exit(EXIT_FAILURE);
    }

    wd = inotify_add_watch(fd, "test.txt",IN_MODIFY);
    
    printf("start\n");
    int lastmaker;
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

                    if (isReturn != return_flag_tmp){
                        for (int i = 0; i <10;i++ ){
                            m_tmp[i] = 0;
                        }
                        m_tmp[0]=lastmaker;
                        printf("마스크 맵 초기화\n");
                        return_flag_tmp = isReturn;
                    }

                    m_flag = 0;
                    splitString(line,Smark,flag);
                    // printf("\nmark : %s\n",Smark);
                    // printf("\nflag : %s\n",flag);

                    // printf("\nmarker : %s\n",Smark);

                    marker = atoi(Smark);

                    if (marker == 17 || marker == 37){
                        marker = 0;
                    }
                    // printf("marker : %d\n",marker);
                    change_marker2pos(marker,&m_row,&m_col);
                    
                    

                    for(int id = 0; id < 10; id ++){
                        if (m_tmp[id] == marker) m_flag = 1;
                    }
                    
                    if (m_flag == 0){
                        lastmaker=marker;
                        markerRecognitionTask->row = m_row;
                        markerRecognitionTask->col = m_col;
                        printf("인큐 시작 marker : %d\n",marker);
                        printf("숫자 디버깅 : ");
                        for(int i =0; i < 10; i ++){
                            printf("%d ",m_tmp[i]);
                        }
                        enqueue(&markerRecognitionLogQueue, markerRecognitionTask);
                        printf("인큐 끝\n");
                        m_tmp[m_idx++] = marker;

                        check_col = markerRecognitionTask->col;
                        check_row = markerRecognitionTask->row;
                    }

                    //printf("check row : %d %d\n",check_row,check_col);

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
                    else if (strcmp(flag,"goleft") == 32){
                        leftFlag = 1;
                        rightFlag = 0;
                        front = 0;
                    }
                    else if (strcmp(flag,"goright") == 32){
                        leftFlag = 0;
                        rightFlag = 1; 
                        front = 0;
                    }
                    else if (strcmp(flag,"stopleft") == 32){
                        leftFlag = 1;
                        rightFlag = 0;
                        front = 1;
                    }
                    else if (strcmp(flag,"stopright") == 32){
                        leftFlag = 0;
                        rightFlag = 1; 
                        front = 1;
                    }
                    //printf("le : %d  ri: %d\n",leftFlag,rightFlag);

                }
                //lastPosition = ftell(file);
                fclose(file);
            }
            i += EVENT_SIZE + event->len;
        }
        
    }

    return NULL;

}
