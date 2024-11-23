#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "robot_moving_event.h"

#define ROW 5
#define COL 9

// TOOD : D231 강의실 다시 확인 후, 한 블럭당 한 인덱스를 맡도록 배열 크기 조정
int D231[ROW][COL] = {
    { 8, 9,  0, 10,11,12, 0, 13,14},
    {-1,-1,  0, -1,-1,-1, 0, -1,-1},
    { 1, 2,  0,  3, 4, 5, 0,  6, 7},
    {-1,-1,  0, -1,-1,-1, 0, -1,-1},
    {-1,-1, -1, -1,-1,-1, 0, -1,-1}
};

int goalRow = 0;
int goalCol = 0;

void tabelNumToCoordinate(int tableNum) {
    int isBreak = 0;
    for (int i = 0 ; i < ROW ; i++) {
        for (int j = 0 ; j < COL ; j++) {
            if (D231[i][j] == tableNum) {
                goalRow = i;
                goalCol = j;
                isBreak = 1;
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
    int direction; // 부모 노드에서 이동방향(상 : 1, 하 : 2, 좌 : 3, 우 : 4, 시작 : 0)
    struct Node* parent; // 부모 노드
} Node;

Node* createNode(int row, int col, int g, int h, int direction, Node* parent) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->row = row;
    newNode->col = col;
    newNode->g = g;
    newNode->h = h;
    newNode->f = g + h;
    newNode->direction = direction;
    newNode->parent = parent;
    return newNode;
}

int isValid(int row, int col) {
    return (row >= 0 && row < ROW && col >= 0 && col < COL && D231[row][col] != -1);
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
    enqueue(&moveCommandQueue, &(node->direction));
}

void aStar() {
    printf("start astar\n");
    while (1) {
        if (isEmpty(&findPathQueue)) continue;

        FindPathTask* findPathTask = dequeue(&findPathQueue);
        int startRow = findPathTask->startRow;
        int startCol = findPathTask->startCol;
        int tableNum = findPathTask->tableNum;

        tabelNumToCoordinate(tableNum);

        Node* shouldTravelArr[ROW * COL];
        int shouldTravelArrSize = 0;
        Node* visitedArr[ROW * COL];
        int visitedArrSize = 0;

        Node* startNode = createNode(startRow, startCol, 0, heuristic(startRow, startCol, goalRow, goalCol), 0, NULL);
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

            // 목표에 도달했는지 확인
            if (currentNode->row == goalRow && currentNode->col == goalCol) {
                printf("destionation access\n");

                pthread_mutex_lock(&enqueueCommendMutex);
                
                printf("start enqueue direction\n");
                enqueueDirection(currentNode);
                startMove();
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
            int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}}; // 상하좌우
            for (int i = 0; i < 4; i++) {
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
                    Node* newNode = createNode(newRow, newCol, gNew, hNew, i+1, currentNode);

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
                }
            }
        }

        printf("목표에 도달할 수 없습니다.\n");
    }
    
}


