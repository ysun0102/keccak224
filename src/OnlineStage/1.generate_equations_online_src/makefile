CC=g++

SRC_FILES=$(wildcard *.cpp)
OBJ_FILES=$(patsubst %.cpp,%.o,$(SRC_FILES))
CUDD_PATH=/home/lt/program/cudd-3.0.0
CFLAG=-c -O3  -I $(CUDD_PATH)/cudd -I $(CUDD_PATH)/util -I $(CUDD_PATH)/dddmp -I $(CUDD_PATH)/mtr -I $(CUDD_PATH)/epd -I $(CUDD_PATH)/st 
LDFLAG= -lpthread -static -L $(CUDD_PATH)/cudd/.libs/ -lcudd -lm
TARGET=generate_equations_online

all:
	$(CC) $(CFLAG) $(SRC_FILES) 
	$(CC) -o $(TARGET)  $(OBJ_FILES) $(LDFLAG) 
.PHONY: clean
clean:
	rm $(OBJ_FILES) $(TARGET) -rf
