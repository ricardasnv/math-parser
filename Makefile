CC=gcc
SRCS=src/*.c
TARGET=mp
LFLAGS=-lreadline -lm
CFLAGS=-g

default:
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LFLAGS)
