target:main

SRC = $(wildcard *.c)
CFLAGS = -g -Wall -ansi

CC := gcc
EXE := main
OBJ := $(patsubst %.c,%.o,$(SRC))

.PHONY:clean

clean:
	rm *.o
test:
	@echo $(SRC)
	@echo $(patsubst %.c,%.o,$(SRC))
$(target):$(OBJ)
	$(CC) -o $(target) $(OBJ) 
$(OBJ):$(SRC)
	$(CC) -c $^


