OUT_DIR=.
LIB_PATH=../../bin
INCLUDE_PATH=../../src
CC=gcc


all: $(OUT_DIR)/main

$(OUT_DIR)/main:
	$(CC) main.c -I$(INCLUDE_PATH) -L$(LIB_PATH) -lipc -o $(OUT_DIR)/$@


clean:
	rm $(OUT_DIR)/main
