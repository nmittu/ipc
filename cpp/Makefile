PWD=$(shell pwd)
OUT_DIR=$(PWD)/../bin/cpp
OBJ_DIR=../bin/cpp/objects
LIB_PATH=../bin
CC=g++
C_CC=gcc
CFLAGS=-c -fPIC
INCLUDE=-I../src -I../common

all: setup $(OUT_DIR)/libIPC_CPP.so

$(OUT_DIR)/libIPC_CPP.so: $(OBJ_DIR)/Connection.o $(OBJ_DIR)/hashtable.o $(OBJ_DIR)/Message.o
	$(CC) $(OBJ_DIR)/Connection.o $(OBJ_DIR)/Message.o $(OBJ_DIR)/hashtable.o -Xlinker -rpath -Xlinker $(PWD)/$(LIB_PATH)  -L$(LIB_PATH) -lipc -shared -o $@

$(OBJ_DIR)/Connection.o:
	$(CC) $(CFLAGS) Connection.cpp $(INCLUDE) -o $@

$(OBJ_DIR)/hashtable.o:
	$(C_CC) $(CFLAGS) ../common/hashtable.c $(INCLUDE) -o $@

$(OBJ_DIR)/Message.o:
	$(CC) $(CFLAGS) Message.cpp $(INCLUDE) -o $@

setup: $(OUT_DIR) $(OBJ_DIR)

$(OUT_DIR):
	mkdir $(OUT_DIR)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

clean:
	rm $(OUT_DIR)/*so $(OBJ_DIR)/*o
