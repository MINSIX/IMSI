SRCS = main.c bluetooth.c
OBJECTS = $(SRCS:.c=.o)
CC = gcc
CFLAGS = -g -Wall
TARGET = test
$(TARGET): $(OBJECTS)
$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)
clean :
rm –f $(OBJECTS)
rm –f $(TARGET)
main.o: main.c common.h
print.o : print.c common.h
input.o : input.c common.h