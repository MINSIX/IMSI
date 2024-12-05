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
} Node;

Node* createNode(int row, int col, int g, int h, Node* parent) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->row = row;
    newNode->col = col;
    newNode->g = g;
    newNode->h = h;
    newNode->f = g + h;
    newNode->parent = parent;
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
    enqueue(&moveDestinationQueue, moveDestinationTask);
    printf("%d %d\n", moveDestinationTask->row, moveDestinationTask->col);
}

void* aStar(void* arg) {
    printf("start astar\n");
    while (1) {
        if (isEmpty(&findPathQueue)) continue;

        FindPathTask* findPathTask = dequeue(&findPathQueue);
        int startRow = nowRow;
        int startCol = nowCol;
        int tableNum = findPathTask->tableNum;

        tabelNumToCoordinate(tableNum);

        Node* shouldTravelArr[D231_ROW * D231_COL];
        int shouldTravelArrSize = 0;
        Node* visitedArr[D231_ROW * D231_COL];
        int visitedArrSize = 0;

        Node* startNode = createNode(startRow, startCol, 0, heuristic(startRow, startCol, goalRow, goalCol), NULL);
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
                    Node* newNode = createNode(newRow, newCol, gNew, hNew, currentNode);

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
                        // TODO : 복귀까지 한 다음에 락을 풀어야할 것 같다.
                        pthread_mutex_unlock(&enqueueCommendMutex);
                        return NULL;
                    }
                }
            }
        }

        printf("목표에 도달할 수 없습니다.\n");
    }
    
}


