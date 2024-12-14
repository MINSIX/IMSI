#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "robot_moving_event.h"

#define D229_ROW 18
#define D229_COL 5

#define D230_ROW 16
#define D230_COL 5

#define D231_ROW 16
#define D231_COL 5

int nowRow = D231_ROW - 1;
int nowCol = D231_COL - 2;

int D229[D229_ROW][D229_COL] = {
    {-1,  0,  0, 0, -1},
    {24,  0, 16, 0,  8},
    {-1,  0,  0, 0, -1},
    {23,  0, 15, 0,  7},
    {-1,  0, -1, 0, -1},
    {22,  0, 14, 0,  6},
    {-1,  0, -1, 0, -1},
    {21,  0, 13, 0,  5},
    {-1,  0, -1, 0, -1},
    {20,  0, 12, 0,  4},
    {-1,  0, -1, 0, -1},
    {19,  0, 11, 0,  3},
    {-1,  0, -1, 0, -1},
    {18,  0, 10, 0,  2},
    {-1,  0, -1, 0, -1},
    {17,  0,  9, 0,  1},
    {-1,  0, -1, 0, -1},
    {-1, -1, -1, 0, -1}
};

int D230[D230_ROW][D230_COL] = {
    {-1,  0,  0, 0, -1},
    {21,  0, 14, 0,  7},
    {-1,  0, -1, 0, -1},
    {20,  0, 13, 0,  6},
    {-1,  0, -1, 0, -1},
    {19,  0, 12, 0,  5},
    {-1,  0, -1, 0, -1},
    {18,  0, 11, 0,  4},
    {-1,  0, -1, 0, -1},
    {17,  0, 10, 0,  3},
    {-1,  0, -1, 0, -1},
    {16,  0,  9, 0,  2},
    {-1,  0, -1, 0, -1},
    {15,  0,  8, 0,  1},
    {-1,  0, -1, 0, -1},
    {-1, -1, -1, 0, -1}
};


int D231[D231_ROW][D231_COL] = {
    {-1,  0,  0, 0, -1},
    {21,  0, 14, 0,  7},
    {-1,  0, -1, 0, -1},
    {20,  0, 13, 0,  6},
    {-1,  0, -1, 0, -1},
    {19,  0, 12, 0,  5},
    {-1,  0, -1, 0, -1},
    {18,  0, 11, 0,  4},
    {-1,  0, -1, 0, -1},
    {17,  0, 10, 0,  3},
    {-1,  0, -1, 0, -1},
    {16,  0,  9, 0,  2},
    {-1,  0, -1, 0, -1},
    {15,  0,  8, 0,  1},
    {-1,  0, -1, 0, -1},
    {-1, -1, -1, 0, -1},
};

int goalRow = 0;
int goalCol = 0;

void tabelNumToCoordinate(int tableNum) {
    int isBreak = 0;
    for (int i = 0 ; i < D231_ROW ; i++) {
        for (int j = 0 ; j < D231_COL ; j++) {
            if (D231[i][j] == tableNum) {
                goalRow = i;
                goalCol = j;
                isBreak = 1;
                printf("goalRow : %d, goalCol : %d\n", i, j);
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
    return (row >= 0 && row < D231_ROW && col >= 0 && col < D231_COL && D231[row][col] == 0);
}

int heuristic(int row, int col, int goalRow, int goalCol) {
    return abs(row - goalRow) + abs(col - goalCol); // 맨해튼 거리

    // return sqrt(pow(row - goalRow, 2) + pow(col - goalCol, 2)); // 유클리드 거리
}

void printPath(Node* node) {
    if (node->parent == NULL) return;
    printPath(node->parent);
    printf("(%d, %d) -> ", node->row, node->col);
}

void enqueueDirection(Node* node) {
    if (node->parent == NULL) return;
    enqueueDirection(node->parent);

    MoveDestinationTask* moveDestinationTask = (MoveDestinationTask*)malloc(sizeof(MoveDestinationTask));
    moveDestinationTask->row = node->row;
    moveDestinationTask->col = node->col;
    moveDestinationTask->direction = node->direction;
    enqueue(&moveDestinationQueue, moveDestinationTask);
    printf("%d %d\n", moveDestinationTask->row, moveDestinationTask->col);
}

void* aStar(void* arg) {
    printf("start astar\n");
    while (1) {
        // if (isEmpty(&findPathQueue)) continue;

        FindPathTask* findPathTask = dequeue(&findPathQueue);
        printf("dequeue: %d", findPathTask->tableNum);
        int startRow = nowRow;
        int startCol = nowCol;
        int tableNum = findPathTask->tableNum;

        tabelNumToCoordinate(tableNum);

        Node* shouldTravelArr[D231_ROW * D231_COL];
        int shouldTravelArrSize = 0;
        Node* visitedArr[D231_ROW * D231_COL];
        int visitedArrSize = 0;

        Node* startNode = createNode(startRow, startCol, 0, heuristic(startRow, startCol, goalRow, goalCol), NULL, 0);
        shouldTravelArr[shouldTravelArrSize++] = startNode;
        
        int isStart = 1;
        while (shouldTravelArrSize > 0) {
            // 가장 낮은 f 값을 가진 노드 찾기
            int minIndex = 0;
            for (int i = 1; i < shouldTravelArrSize; i++) {
                if (shouldTravelArr[i]->f < shouldTravelArr[minIndex]->f) {
                    minIndex = i;
                }
            }

            Node* currentNode = shouldTravelArr[minIndex];

            // 현재 노드를 openList에서 제거하고 closedList에 추가
            for (int i = minIndex; i < shouldTravelArrSize - 1; i++) {
                shouldTravelArr[i] = shouldTravelArr[i + 1];
            }
            shouldTravelArrSize--;
            visitedArr[visitedArrSize++] = currentNode;

            // 이웃 노드 탐색
            int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}}; // 상하좌우
            for (int i = 0; i < 4; i++) {
                if (isStart) {
                    if (nowRobotDir+1 == i+1 || nowRobotDir-1 == i+1) {
                        isStart = 0;
                        continue;
                    }
                }
                int newRow = currentNode->row + directions[i][0];
                int newCol = currentNode->col + directions[i][1];

                if (isValid(newRow, newCol)) {
                    // 이미 방문한 적 있는 노드인지 확인
                    int foundInClosed = 0;
                    for (int j = 0; j < visitedArrSize; j++) {
                        if (visitedArr[j]->row == newRow && visitedArr[j]->col == newCol) {
                            foundInClosed = 1;
                            break;
                        }
                    }
                    if (foundInClosed) continue;

                    // 출발 노드부터 현재 노드까지 거리 계산
                    int gNew = currentNode->g + 1;
                    // 휴리스틱 계산
                    int hNew = heuristic(newRow, newCol, goalRow, goalCol);
                    Node* newNode = createNode(newRow, newCol, gNew, hNew, currentNode, i+1);

                    // openList에 있는지 확인
                    // 있으면 기존의 경로와 새 경로 비교 후 최단 경로로 갱신
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
                } else {
                    // 목표에 도달했는지 확인
                    if (newRow == goalRow && newCol == goalCol) {
                        printf("destionation access\n");

                        pthread_mutex_lock(&enqueueCommendMutex);
                        
                        printf("start enqueue direction\n");
                        enqueueDirection(currentNode);
                        pthread_mutex_unlock(&enqueueCommendMutex);
                        return NULL;
                    }
                }
            }
        }

        printf("목표에 도달할 수 없습니다.\n");
    }
    
}

// aStar를 호출하는 코드를 만들고, aStar 호출 영역을 임계영역으로 지정한다.
// 첫번째 동작인 테이블로 이동을 위한 aStar 호출 후 버튼 눌림이 올 때 까지 cond_wait
// 목표위치가 아닌 다른 마커에 도달하면 move_robot.c 내에서 큐 모두 삭제 및 aStar를 호출하여 새 경로 큐에 삽입을 한다.
// 버튼이 눌리면 스레드를 꺠우고, 복귀를 위한 aStar 호출 후 복귀 성공할 때 까지 cond_wait
// 복귀 성공 후에 스레드를 꺠우고 임계영역을 빠져나간다.

// 전역변수 : pushButtonFlag, cond 변수, 작업이 복귀임을 나타내는 플래그 값


