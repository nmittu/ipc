#include <stdio.h>
#include <string.h>
#include "ipc.h"

int recived = 0;

void recive(Message* msg){
  printf("Recived data: %s\n", msg->data);

  recived = 1;
}

void parent(Connection* conn, pid_t pid){
  connectionSetCallback(conn, recive);
  connectionStartAutoDispatch(conn);

  while (!recived) {
    usleep(100);
  }
}

void child(){
  Connection* conn = connectionConnect("ipcdemo", CONN_TYPE_PID);
  char* str = "This string was sent over IPC using named pipes!";
  Message* msg = messageCreate(str, strlen(str));
  messageSetPID(msg, getppid());
  connectionSend(conn, msg);
  messageDestroy(msg);

  //wait for message to send until you free data

  {
    int i;
    for(i = 0; i < 5; i++){
      usleep(500);
    }
  }
}

int main(int argc, char const *argv[]) {
  pid_t pid = fork();

  Connection* conn = connectionCreate("ipcdemo", CONN_TYPE_PID);

  if (pid == 0){
    child();
  }else{
    parent(conn, pid);
  }

  connectionDestroy(conn);

  return 0;
}
