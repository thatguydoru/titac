CC := gcc
SRC := titac.c
APP_NAME := titac
ARGS := -Wall -O1

all: build run clean

build: $(SRC)
	$(CC) $(ARGS) $(SRC) -o $(APP_NAME)

run: $(APP_NAME)
	./$(APP_NAME)

clean: $(APP_NAME)
	rm $(APP_NAME)
