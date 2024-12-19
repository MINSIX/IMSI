#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "robot_moving_event.h"

#define FINAL_ROW 7
#define FINAL_COL 4

#define D229_DEMO_ROW 7
#define D229_DEMO_COL 4

int nowRobotRow = 6;
int nowRobotCol = 3;
int nowRobotDir = 7;
pthread_cond_t waitFindPath = PTHREAD_COND_INITIALIZER;
pthread_mutex_t waitFindPathMutex = PTHREAD_MUTEX_INITIALIZER;

int finalMap[FINAL_ROW][FINAL_COL] = {
    {6,  0,  0,  5},
    {0, -1,  0,  0},
    {0,  0, -1,  0},
    {3,  0,  0,  4},
    {0, -1, -1, -1},
    {0, -1, -1, -1},
    {2,  0,  0,  1}
};
int D229_Demo[D229_DEMO_ROW][D229_DEMO_COL] = {
    {-1, -1, -1,  4},
    {-1, -1,  0,  0},
    {-1,  0, -1,  0},
    {-3,  0,  0,  3},
    { 0, -1, -1, -1},
    { 0, -1, -1, -1},
    { 2,  0,  0,  1}
};

// <배열 값 규칙>
// 갈 수 없는 곳 : -1
// 시작점 값 : 1
// 테이블 값 : 1이 아닌 양의 상수
// 길 : 0
// 교차로 : -1이 아닌 음의 상수
// 교차로이면서 테이블 : ?

// <마커 종류>
// 시작점
// 테이블
// 교차로

int finalGoalRow = 0;
int finalGoalCol = 0;

void tabelNumToCoordinate(int tableNum) {
    int isBreak = 0;
    for (int i = 0 ; i < FINAL_ROW ; i++) {
        for (int j = 0 ; j < FINAL_COL ; j++) {
            if (finalMap[i][j] == tableNum) {
                finalGoalRow = i;
                finalGoalCol = j;
                isBreak = 1;
                printf("finalGoalRow : %d, finalGoalCol : %d\n", i, j);
                break;
            }
        }
        if (isBreak) break;
    }

    if (!isBreak) {
        printf("잘못된 테이블 번호입니다!\n");
        exit(EXIT_FAILURE);
    }
}

// 구조체 정의
typedef struct Node {
    int row, col; // 노드의 행, 열 좌표
    int g; // 시작점부터 현재 노드까지 비용
    int h; // 휴리스틱 (현재 노드부터 목적지까지 비용)
    int f; // g + h
    struct Node* parent; // 부모 노드
    int direction; // 도달 방향(1: 상, 2: 하, 3: 좌, 4: 우, 0: 출발 위치)
} Node;

Node* createNode(int row, int col, int g, int h, Node* parent, int direction) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->row = row;
    newNode->col = col;
    newNode->g = g;
    newNode->h = h;
    newNode->f = g + h;
    newNode->parent = parent;
    newNode->direction = direction;
    return newNode;
}

int isValid(int row, int col) {
    return (row >= 0 && row < FINAL_ROW && col >= 0 && col < FINAL_COL && finalMap[row][col] != -1);
}

int heuristic(int row, int col, int finalGoalRow, int finalGoalCol) {
    return abs(row - finalGoalRow) + abs(col - finalGoalCol); // 맨해튼 거리

    // return sqrt(pow(row - finalGoalRow, 2) + pow(col - finalGoalCol, 2)); // 유클리드 거리
}

void printPath(Node* node) {
    if (node->parent == NULL) return;
    printPath(node->parent);
    printf("(%d, %d) -> ", node->row, node->col);
}

void enqueueDirection(Node* node) {
    if (node->parent == NULL) return;
    enqueueDirection(node->parent);

    if (finalMap[node->row][node->col]== 0) {
        // 0(길)은 마커가 없음
        printf("road : %d %d\n", node->row, node->col);
        return;
    }

    MoveDestinationTask* moveDestinationTask = (MoveDestinationTask*)malloc(sizeof(MoveDestinationTask));
    moveDestinationTask->markerNum = finalMap[node->row][node->col];
    moveDestinationTask->row = node->row;
    moveDestinationTask->col = node->col;
    moveDestinationTask->direction = node->direction;
    enqueue(&moveDestinationQueue, moveDestinationTask);
    printf("marker : %d %d\n", moveDestinationTask->row, moveDestinationTask->col);
}
// 대각선 이동 제한 여부를 확인하는 함수
int isValidDiagonal(int currentRow, int currentCol, int newRow, int newCol) {
    if ((currentRow == 2 && currentCol == 1 && newRow == 1 && newCol == 2) ||
        (currentRow == 1 && currentCol == 2 && newRow == 2 && newCol == 1)) {
        return 1;
    }
    return 0;
}
int isValidVertical(int currentRow, int currentCol, int newRow, int newCol) {
    if ((currentRow == 0 && currentCol == 2 && newRow == 1 && newCol == 2) ||
        (currentRow == 1 && currentCol == 2 && newRow == 0 && newCol == 2) ||
        (currentRow == 1 && currentCol == 3 && newRow == 1 && newCol == 2) ||
        (currentRow == 1 && currentCol == 2 && newRow == 1 && newCol == 3) ||
        (currentRow == 2 && currentCol == 0 && newRow == 2 && newCol == 1) ||
        (currentRow == 2 && currentCol == 1 && newRow == 2 && newCol == 0) ||
        (currentRow == 3 && currentCol == 1 && newRow == 2 && newCol == 1)  ||
        (currentRow == 2 && currentCol == 1 && newRow == 3 && newCol == 1)) {
        return 0;
    }
    return 1;
}

void aStar(FindPathTask* findPathTask) {
    printf("astar!\n");
    int startRow = nowRobotRow;
    int startCol = nowRobotCol;
    int tableNum = findPathTask->tableNum;

    tabelNumToCoordinate(tableNum);

    Node* shouldTravelArr[FINAL_ROW * FINAL_COL];
    int shouldTravelArrSize = 0;
    Node* visitedArr[FINAL_ROW * FINAL_COL];
    int visitedArrSize = 0;

    Node* startNode = createNode(startRow, startCol, 0, heuristic(startRow, startCol, finalGoalRow, finalGoalCol), NULL, 0);
    shouldTravelArr[shouldTravelArrSize++] = startNode;

    while (shouldTravelArrSize > 0) {
        // 가장 낮은 f 값을 가진 노드 찾기
        int minIndex = 0;
        for (int i = 1; i < shouldTravelArrSize; i++) {
            if (shouldTravelArr[i]->f < shouldTravelArr[minIndex]->f) {
                minIndex = i;
            }
        }

        Node* currentNode = shouldTravelArr[minIndex];
        int currentNodeNum = finalMap[currentNode->row][currentNode->col];
        if (currentNode->row == finalGoalRow && currentNode->col == finalGoalCol) {
            printf("Destination reached\n");

            pthread_mutex_lock(&enqueueCommendMutex);
            enqueueDirection(currentNode);
            pthread_mutex_unlock(&enqueueCommendMutex);
            return;
        }

        // 현재 노드를 openList에서 제거하고 closedList에 추가
        for (int i = minIndex; i < shouldTravelArrSize - 1; i++) {
            shouldTravelArr[i] = shouldTravelArr[i + 1];
        }
        shouldTravelArrSize--;
        visitedArr[visitedArrSize++] = currentNode;

        // 이웃 노드 탐색
        int directions[8][2] = {{-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}};
        for (int i = 0; i < 8; i++) {
            int newRow = currentNode->row + directions[i][0];
            int newCol = currentNode->col + directions[i][1];

            if (!isValid(newRow, newCol)) continue;

            int newNodeNum = finalMap[newRow][newCol];

            // 대각선 및 수직 이동 제한 조건
            if ((currentNodeNum == 0 && newNodeNum == 0)) {
                if((i % 2 == 1 && !isValidDiagonal(currentNode->row, currentNode->col, newRow, newCol))) {
                    continue;
                }
                if((i % 2 == 0 && !isValidVertical(currentNode->row, currentNode->col, newRow, newCol))) {
                    continue;
                }
            }

            // 이미 방문한 노드인지 확인
            int foundInClosed = 0;
            for (int j = 0; j < visitedArrSize; j++) {
                if (visitedArr[j]->row == newRow && visitedArr[j]->col == newCol) {
                    foundInClosed = 1;
                    break;
                }
            }
            if (foundInClosed) continue;

            int gNew = currentNode->g + 1;
            int hNew = heuristic(newRow, newCol, finalGoalRow, finalGoalCol);
            Node* newNode = createNode(newRow, newCol, gNew, hNew, currentNode, i + 1);

            // openList에 있는지 확인하고 최단 경로 갱신
            int foundInOpen = 0;
            for (int j = 0; j < shouldTravelArrSize; j++) {
                if (shouldTravelArr[j]->row == newRow && shouldTravelArr[j]->col == newCol) {
                    foundInOpen = 1;
                    if (gNew < shouldTravelArr[j]->g) {
                        shouldTravelArr[j]->g = gNew;
                        shouldTravelArr[j]->f = gNew + hNew;
                        shouldTravelArr[j]->parent = currentNode;
                    }
                    break;
                }
            }
            if (!foundInOpen) {
                shouldTravelArr[shouldTravelArrSize++] = newNode;
            }
        }
    }
    printf("Cannot reach the goal.\n");
}



// void aStar(FindPathTask* findPathTask) {
//     printf("astar!\n");
//     int startRow = nowRobotRow;
//     int startCol = nowRobotCol;
//     int tableNum = findPathTask->tableNum;

//     tabelNumToCoordinate(tableNum);

//     Node* shouldTravelArr[D229_DEMO_ROW * D229_DEMO_COL];
//     int shouldTravelArrSize = 0;
//     Node* visitedArr[D229_DEMO_ROW * D229_DEMO_COL];
//     int visitedArrSize = 0;

//     Node* startNode = createNode(startRow, startCol, 0, heuristic(startRow, startCol, finalGoalRow, finalGoalCol), NULL, 0);
//     shouldTravelArr[shouldTravelArrSize++] = startNode;
    
//     while (shouldTravelArrSize > 0) {
//         // 가장 낮은 f 값을 가진 노드 찾기
//         int minIndex = 0;
//         for (int i = 1; i < shouldTravelArrSize; i++) {
//             if (shouldTravelArr[i]->f < shouldTravelArr[minIndex]->f) {
//                 minIndex = i;
//             }
//         }

//         Node* currentNode = shouldTravelArr[minIndex];
//         int currentNodeNum = D229_Demo[currentNode->row][currentNode->col];
//         printf("currentNodeRow : %d, currentNodeCol : %d\n", currentNode->row, currentNode->col);
//         if (currentNode->row == finalGoalRow && currentNode->col == finalGoalCol) {
//             printf("destionation access\n");

//             pthread_mutex_lock(&enqueueCommendMutex);
            
//             printf("start enqueue direction\n");
//             enqueueDirection(currentNode);
//             pthread_mutex_unlock(&enqueueCommendMutex);
//             return;
//         }

//         // 현재 노드를 openList에서 제거하고 closedList에 추가
//         for (int i = minIndex; i < shouldTravelArrSize - 1; i++) {
//             shouldTravelArr[i] = shouldTravelArr[i + 1];
//         }
//         shouldTravelArrSize--;
//         visitedArr[visitedArrSize++] = currentNode;

//         // 이웃 노드 탐색
//         int directions[8][2] = {{-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}}; // 상 우상 우 우하 하 좌하 좌 좌상
//         for (int i = 0; i < 8; i++) {
//             if (nowRobotDir - 4 == i+1 || nowRobotDir + 4 == i+1) {
//                 continue;
//             }
//             int newRow = currentNode->row + directions[i][0];
//             int newCol = currentNode->col + directions[i][1];
//             if (isValid(newRow, newCol)) {
//                 int newNodeNum = D229_Demo[newRow][newCol];

//                 if(currentNodeNum == 0 && newNodeNum == 0) {
//                     // 길끼리 대각선으로 이어지면 안되는 경우
//                     if((i+1) % 2 == 0) {
//                         // 대각선 경로를 기록하여 기록한 경로가 아니면 continue 하도록 설정
//                         if ((!(currentNode->row == 2 && currentNode ->col == 1 && newRow == 1 && newCol == 2) 
//                                 || (currentNode->row == 1 && currentNode ->col == 2 && newRow == 2 && newCol == 1))) {
//                             continue;
//                         }
//                     } 
//                     // 길끼리 상하좌우로 이어지면 안되는 경우
//                     else {
//                         if (((currentNode->row == 1 && currentNode ->col == 3 && newRow == 1 && newCol == 2) 
//                                 || (currentNode->row == 1 && currentNode ->col == 2 && newRow == 1 && newCol == 3))) {
//                             continue;
//                         }
//                     }
//                 }

//                 // 이미 방문한 적 있는 노드인지 확인
//                 int foundInClosed = 0;
//                 for (int j = 0; j < visitedArrSize; j++) {
//                     if (visitedArr[j]->row == newRow && visitedArr[j]->col == newCol) {
//                         foundInClosed = 1;
//                         break;
//                     }
//                 }
//                 if (foundInClosed) continue;

//                 // 출발 노드부터 현재 노드까지 거리 계산
//                 int gNew = currentNode->g + 1;
//                 // 휴리스틱 계산
//                 int hNew = heuristic(newRow, newCol, finalGoalRow, finalGoalCol);
//                 Node* newNode = createNode(newRow, newCol, gNew, hNew, currentNode, i+1);

//                 // openList에 있는지 확인
//                 // 있으면 기존의 경로와 새 경로 비교 후 최단 경로로 갱신
//                 int foundInOpen = 0;
//                 for (int j = 0; j < shouldTravelArrSize; j++) {
//                     if (shouldTravelArr[j]->row == newRow && shouldTravelArr[j]->col == newCol) {
//                         foundInOpen = 1;
//                         if (gNew < shouldTravelArr[j]->g) {
//                             shouldTravelArr[j]->g = gNew;
//                             shouldTravelArr[j]->f = gNew + hNew;
//                             shouldTravelArr[j]->parent = currentNode;
//                         }
//                         break;
//                     }
//                 }
//                 if (!foundInOpen) {
//                     shouldTravelArr[shouldTravelArrSize++] = newNode;
//                 }
//             }
//         }
//     }
//     printf("목표에 도달할 수 없습니다.\n");
// }

void* findShortestPath(void* arg) {
    printf("findShortestPath!\n");
    while(1) {
        FindPathTask* findPathTask = dequeue(&findPathQueue);
        printf("dequeue: %d\n", findPathTask->tableNum);

        aStar(findPathTask);
        // 로봇 출발 및 복귀가 완료 될 떄까지 대기
        // pthread_mutex_lock(&waitFindPathMutex);
        // if(waitFindPathFlag){
        //     pthread_cond_wait(&waitFindPath, &waitFindPathMutex);
        // }
        // pthread_mutex_unlock(&waitFindPathMutex);
    }
}

// aStar를 호출하는 코드를 만들고, aStar 호출 영역을 임계영역으로 지정한다.
// 첫번째 동작인 테이블로 이동을 위한 aStar 호출 후 버튼 눌림이 올 때 까지 cond_wait
// 목표위치가 아닌 다른 마커에 도달하면 move_robot.c 내에서 큐 모두 삭제 및 aStar를 호출하여 새 경로 큐에 삽입을 한다.
// 버튼이 눌리면 스레드를 꺠우고, 복귀를 위한 aStar 호출 후 복귀 성공할 때 까지 cond_wait
// 복귀 성공 후에 스레드를 꺠우고 임계영역을 빠져나간다.

// 전역변수 : pushButtonFlag, cond 변수, 작업이 복귀임을 나타내는 플래그 값


