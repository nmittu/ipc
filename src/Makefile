OUT_DIR=$(shell pwd)/../bin
OBJ_DIR=../bin/objects
CC=gcc
CFLAGS=-c


all: setup $(OUT_DIR)/libipc.so

$(OUT_DIR)/libipc.so: $(OBJ_DIR)/connection.o $(OBJ_DIR)/message.o
	$(CC) $(OBJ_DIR)/connection.o $(OBJ_DIR)/message.o -shared -o $@

$(OBJ_DIR)/connection.o:
	$(CC) $(CFLAGS) connection.c -o $@

$(OBJ_DIR)/message.o:
	$(CC) $(CFLAGS) message.c -o $@

setup: $(OUT_DIR) $(OBJ_DIR)

$(OUT_DIR):
	mkdir $(OUT_DIR)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

clean:
	rm $(OUT_DIR)/*so $(OBJ_DIR)/*o
