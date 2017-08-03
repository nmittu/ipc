#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "ipc.h"

int recived = 0;

void recive(Message* msg){
  printf("Recived data: %s\nTo PID: %d\n", msg->data, msg->pid);

  recived = 1;
}

void parent(Connection* conn, pid_t pid){
  while (!recived) {
    usleep(100);
  }
  connectionStopAutoDispatch(conn);
  connectionDestroy(conn);
}

void child(Connection* conn){
  connectionStopAutoDispatch(conn);
  char* str = "This string was sent over IPC using named pipes!";
  Message* msg = messageCreate(str, strlen(str)+1);
  messageSetPID(msg, getppid());
  connectionSend(conn, msg);
  messageDestroy(msg);

  int i;
  for (i = 0; i < 50; i++) {
    usleep(100);
  }
  connectionDestroy(conn);
}

int main(int argc, char const *argv[]) {
  Connection* conn = connectionCreate("ipcdemo", CONN_TYPE_PID);

  connectionSetCallback(conn, recive);
  connectionStartAutoDispatch(conn);

  pid_t pid = fork();

  if (pid == 0){
    child(conn);
  }else{
    parent(conn, pid);

    connectionDestroy(conn);
  }

  return 0;
}
