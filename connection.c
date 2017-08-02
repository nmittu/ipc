#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "connection.h"

#define MAX_CB 50

//TODO: Implement hashmap
struct ConnCallbackElement {
  Connection* conn;
  ConnectionCallback cb;
  pthread_mutex_t mutex;
  pthread_t tid;
  struct dispatcherArgs* args;
};

struct dispatcherArgs{
  ConnectionCallback cb;
  int type;
  char* path;
  int* cont;
  pthread_mutex_t* mutex;
};

struct cbCallerArgs{
  Message* msg;
  ConnectionCallback cb;
};

struct writerArgs{
  Message* msg;
  char* path;
};

struct ConnCallbackElement* cbs[MAX_CB];
int numCallbacks;

void* writer(void* args){
  struct writerArgs* argz = args;

  int fd = open(argz->path, O_WRONLY);

  size_t len = sizeof(Message) + argz->msg->len;
  char* data = malloc(len);

  memcpy(data, argz->msg, sizeof(Message));
  memcpy((data + sizeof(Message)), argz->msg->data, argz->msg->len);

  write(fd, data, len);
  close(fd);

  free(data);
  free(argz->path);
  free(argz->msg->data);
  messageDestroy(argz->msg);
  free(argz);
}

void* cbCaller(void* args){
  struct cbCallerArgs* argz = args;

  (argz->cb)(argz->msg);

  messageDestroy(argz->msg);
  free(argz);
}

void dispatch(ConnectionCallback cb, Message* msg){
  struct cbCallerArgs* cbargs = malloc(sizeof(struct cbCallerArgs));
  cbargs->cb = cb;
  cbargs->msg = msg;

  pthread_t tid;
  pthread_create(&tid, NULL, cbCaller, cbargs);
}

void* dispatcher(void* args){
  struct dispatcherArgs* argz = args;

  pthread_mutex_lock(argz->mutex);
  int fd = open(argz->path, O_RDONLY);
  while (*(argz->cont)){
    pthread_mutex_unlock(argz->mutex);

    char* data = malloc(MAX_MSG_SIZE);

    read(fd, data, MAX_MSG_SIZE);

    Message* msg = (Message*) data;
    data += sizeof(Message);
    msg->data = data;

    switch (msg->type) {
      case CONN_TYPE_ALL:
        dispatch(argz->cb, msg);
        break;
      case CONN_TYPE_SUB:
        //TODO check subscriptions
        dispatch(argz->cb, msg);
        break;
      case CONN_TYPE_PID:
        if (msg->pid == getpid()){
          dispatch(argz->cb, msg);
        } else {
          messageDestroy(msg);
        }
    }

    pthread_mutex_lock(argz->mutex);
  }

  close(fd);
}

int findFreeCBSlot(){
  if(numCallbacks < MAX_CB){
    int i;
    for (i = 0; i < MAX_CB; i++){
      if (cbs[i] == NULL){
        return i;
      }
    }
  }
  return -1;
}

int findInCBSlot(Connection* conn){
  int i;
  for (i = 0; i < MAX_CB; i++){
    if (cbs[i]->conn == conn){
      return i;
    }
  }
  return -1;
}

Connection* connectionCreate(char* name, int type){
  Connection* ret = malloc(sizeof(Connection));
  ret->name = malloc(strlen(name)+1);
  memcpy(ret->name, name, strlen(name)+1);
  ret->type = type;

  char* path = malloc(strlen(name) + 1 + 6);
  memcpy(path, "/tmp/", 6);
  strcat(path, name);
  mkfifo(path, 0666);
  free(path);

  return ret;
}

Connection* connectionConnect(char* name, int type){
  Connection* ret = malloc(sizeof(Connection));
  ret->name = malloc(strlen(name) + 1);
  memcpy(ret->name, name, strlen(name) + 1);
  ret->type = type;

  return ret;
}

void connectionStartAutoDispatch(Connection* conn){
  int i = findInCBSlot(conn);

  cbs[i]->args = malloc(sizeof(struct dispatcherArgs));

  cbs[i]->args->mutex = malloc(sizeof(pthread_mutex_t));

  pthread_mutex_lock(cbs[i]->args->mutex);
  cbs[i]->args->cont = malloc(sizeof(int));
  *(cbs[i]->args->cont) = 1;
  pthread_mutex_unlock(cbs[i]->args->mutex);

  cbs[i]->args->path = malloc(strlen(conn->name) + 1 + 6);
  memcpy(cbs[i]->args->path, "/tmp/", 6);
  strcat(cbs[i]->args->path, conn->name);

  cbs[i]->args->cb = cbs[i]->cb;

  pthread_create(&(cbs[i]->tid), NULL, dispatcher, cbs[i]->args);
}

void connectionStopAutoDispatch(Connection* conn){
  int i = findInCBSlot(conn);

  pthread_mutex_lock(cbs[i]->args->mutex);
  *(cbs[i]->args->cont) = 0;
  pthread_mutex_unlock(cbs[i]->args->mutex);

  pthread_join(cbs[i]->tid, NULL);

  free(cbs[i]->args->mutex);
  free(cbs[i]->args->cont);
  free(cbs[i]->args->path);
  free(cbs[i]->args);
}

void connectionSetCallback(Connection* conn, ConnectionCallback cb){
  int i = findFreeCBSlot();

  cbs[i] = malloc(sizeof(struct ConnCallbackElement));
  cbs[i]->conn = conn;
  cbs[i]->cb = cb;
}

ConnectionCallback connectionGetCallback(Connection* conn){
  int i = findInCBSlot(conn);

  return cbs[i]->cb;
}

void connectionRemoveCallback(Connection* conn){
  int i = findInCBSlot(conn);

  free(cbs[i]);
  cbs[i] = NULL;
}

void connectionSend(Connection* conn, Message* msg){
  char* path = malloc(strlen(conn->name) + 1 + 6);
  memcpy(path, "/tmp/", 6);
  strcat(path, conn->name);

  struct writerArgs* args = malloc(sizeof(struct writerArgs));
  args->msg = malloc(sizeof(Message));
  memcpy(args->msg, msg, sizeof(Message));

  args->msg->data = malloc(msg->len);
  memcpy(args->msg->data, msg->data, msg->len);

  args->path = path;

  pthread_t tid;
  pthread_create(&tid, NULL, writer, args);
}

void connectionDestroy(Connection* conn){
  char* path = malloc(strlen(conn->name) + 1 + 6);
  memcpy(path, "/tmp/", 6);
  strcat(path, conn->name);
  unlink(path);

  free(conn->name);
  free(conn);
}
