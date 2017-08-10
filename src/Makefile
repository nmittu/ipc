OUT_DIR=$(shell pwd)/../bin
OBJ_DIR=../objects
CC=gcc
CFLAGS=-c


all: $(OUT_DIR)/libipc.so

$(OUT_DIR)/libipc.so: $(OBJ_DIR)/connection.o $(OBJ_DIR)/message.o
	$(CC) $(OBJ_DIR)/connection.o $(OBJ_DIR)/message.o -shared -o $@

$(OBJ_DIR)/connection.o:
	$(CC) $(CFLAGS) connection.c -o $@

$(OBJ_DIR)/message.o:
	$(CC) $(CFLAGS) message.c -o $@

clean:
	rm $(OUT_DIR)/*so $(OBJ_DIR)/*o
