OUT_DIR=.
OBJ_DIR=./objects
CC=gcc
CFLAGS=-c
LIB_PATH=../bin
INCLUDE=-I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/$(JAVA_ARCH) -I../src
PACKAGE=com/mittudev/ipc

all: $(OUT_DIR)/libIPC.so $(OUT_DIR)/IPC.jar

$(OUT_DIR)/libIPC.so: $(OBJ_DIR)/com_mittudev_ipc_Connection.o $(OBJ_DIR)/com_mittudev_ipc_Message.o
	$(CC) $(OBJ_DIR)/com_mittudev_ipc_Connection.o $(OBJ_DIR)/com_mittudev_ipc_Message.o -L$(LIB_PATH) -lipc -shared -o $@

$(OBJ_DIR)/com_mittudev_ipc_Connection.o:
	$(CC) $(CFLAGS) com_mittudev_ipc_Connection.c $(INCLUDE) -o $@

$(OBJ_DIR)/com_mittudev_ipc_Message.o:
	$(CC) $(CFLAGS) com_mittudev_ipc_Message.c $(INCLUDE) -o $@

$(OUT_DIR)/IPC.jar: $(PACKAGE)/Connection.class $(PACKAGE)/Message.class $(PACKAGE)/ConnectionCallback.class
	jar -cf $@ $(PACKAGE)/*class

$(PACKAGE)/Connection.class:
	javac $(PACKAGE)/Connection.java

$(PACKAGE)/Message.class:
	javac $(PACKAGE)/Message.java

$(PACKAGE)/ConnectionCallback.class:
	javac $(PACKAGE)/ConnectionCallback.java

examples: Writer.class Reader.class

Writer.class:
	javac Writer.java

Reader.class:
	javac Reader.java

clean:
	rm *class
	rm $(PACKAGE)/*class
	rm $(OUT_DIR)/IPC.jar $(OUT_DIR)/libIPC.so
	rm $(OBJ_DIR)/*o
