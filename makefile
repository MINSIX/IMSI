# Makefile

# 컴파일러와 플래그 정의
CC = gcc
CFLAGS = -Wall -Wextra -g -pthread
LDFLAGS = -pthread -lm 

# 소스 파일
SRCS = main.c find_shortest_path.c move_robot.c task_queue.c

# 실행 파일 이름
TARGET = find_shortest_path

# 기본 규칙
all: $(TARGET)

# 실행 파일 생성 규칙
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# 클린 규칙
clean:
	rm -f $(TARGET)

.PHONY: all clean
