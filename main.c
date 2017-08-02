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
  connectionSetCallback(conn, recive);
  printf("recieving\n");
  connectionStartAutoDispatch(conn);

  while (!recived) {
    usleep(100);
  }
}

void child(){
  Connection* conn = connectionConnect("ipcdemo", CONN_TYPE_PID);
  char* str = "This string was sent over IPC using named pipes!";
  Message* msg = messageCreate(str, strlen(str)+1);
  messageSetPID(msg, getppid());
  usleep(500000);
  printf("sending\n");
  connectionSend(conn, msg);
  messageDestroy(msg);
}

int main(int argc, char const *argv[]) {

  pid_t pid = fork();

  if (pid == 0){
    child();
  }else{
    Connection* conn = connectionCreate("ipcdemo", CONN_TYPE_PID);
    parent(conn, pid);

    connectionDestroy(conn);
  }

  return 0;
}
