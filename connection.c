#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "connection.h"

#define MAX_CB 50
#define SUBS_LEN 10

//TODO: Implement hashmap
struct ConnCallbackElement {
  Connection* conn;
  ConnectionCallback cb;
  pthread_t tid;
  struct dispatcherArgs* args;
};

struct dispatcherArgs{
  ConnectionCallback cb;
  int type;
  char* path;
  int* cont;
  char** subs;
  int* numSubs;
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

int nextEmpty(char** strs, int len){
  int i;
  for (i = 0; i < len; i++) {
    if(strs[i] == NULL){
      return i;
    }
  }
  return -1;
}

int findEqual(char** strs, char* other, int len){
  int i;
  for (i = 0; i < len; i++){
    if(strcmp(strs[i], other) == 0){
      return i;
    }
  }
  return -1;
}

void* writer(void* args){
  struct writerArgs* argz = args;

  int fd = open(argz->path, O_WRONLY);


  size_t len = sizeof(Message) + argz->msg->len;
  char* data = malloc(len);

  memcpy(data, argz->msg, sizeof(Message));
  memcpy((data + sizeof(Message)), argz->msg->data, argz->msg->len);
  if (argz->msg->type == CONN_TYPE_SUB){
    data = realloc(data, len + sizeof(size_t) + strlen(argz->msg->subject) + 1);
    size_t sub_len = strlen(argz->msg->subject) + 1;
    memcpy((data + len), &sub_len, sizeof(size_t));
    memcpy((data + len + sizeof(size_t)), argz->msg->subject, strlen(argz->msg->subject) + 1);
    len += sizeof(size_t) + strlen(argz->msg->subject) + 1;
  }

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

  if(argz->msg->type == CONN_TYPE_SUB){
    free(argz->msg->subject);
  }
  free(argz->msg->data);
  free(argz->msg);
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


  int fd = open(argz->path, O_RDONLY | O_NONBLOCK);

  while (*(argz->cont)){

    Message* msg = malloc(sizeof(Message));
    read(fd, msg, sizeof(Message));

    char* data = malloc(msg->len);
    read(fd, data, msg->len);
    msg->data = data;

    if(msg->type == CONN_TYPE_SUB){
      size_t* sub_len = malloc(sizeof(size_t));
      read(fd, sub_len, sizeof(size_t));

      char* subject = malloc(*sub_len);
      read(fd, subject, *sub_len);
      msg->subject = subject;
    }

    switch (msg->type) {
      case CONN_TYPE_ALL:
        dispatch(argz->cb, msg);
        break;
      case CONN_TYPE_SUB:
        if (findEqual(argz->subs, msg->subject, *(argz->numSubs)) != -1){
          dispatch(argz->cb, msg);
        }else{
          free(msg->subject);
          free(msg);
          free(data);
        }
        break;
      case CONN_TYPE_PID:
        if (msg->pid == getpid()){
          dispatch(argz->cb, msg);
        } else {
          free(msg);
          free(data);
        }
        break;
      default:
        free(msg);
        free(data);

      usleep(100);
    }

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
  mkfifo(path, 0777);
  free(path);

  ret->subscriptions = malloc(sizeof(char*) * SUBS_LEN);

  int i;
  for (i = 0; i < SUBS_LEN; i++){
    ret->subscriptions[i] = NULL;
  }

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


  cbs[i]->args->cont = malloc(sizeof(int));
  *(cbs[i]->args->cont) = 1;

  cbs[i]->args->path = malloc(strlen(conn->name) + 1 + 6);
  memcpy(cbs[i]->args->path, "/tmp/", 6);
  strcat(cbs[i]->args->path, conn->name);

  cbs[i]->args->cb = cbs[i]->cb;

  cbs[i]->args->subs = conn->subscriptions;
  cbs[i]->args->numSubs = &(conn->numSubs);

  pthread_create(&(cbs[i]->tid), NULL, dispatcher, cbs[i]->args);
}

void connectionStopAutoDispatch(Connection* conn){
  int i = findInCBSlot(conn);

  *(cbs[i]->args->cont) = 0;

  pthread_join(cbs[i]->tid, NULL);

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

void connectionSubscribe(Connection* conn, char* subject){
  int i = nextEmpty(conn->subscriptions, conn->numSubs);

  if (i == -1){
    conn->subscriptions = realloc(conn->subscriptions, conn->numSubs + SUBS_LEN);
    int i;
    for(i = conn->numSubs; i < conn->numSubs + SUBS_LEN; i++){
      conn->subscriptions[i] = NULL;
    }
    conn->numSubs += SUBS_LEN;
    connectionSubscribe(conn, subject);
    return;
  }

  conn->subscriptions[i] = malloc(strlen(subject) + 1);
  memcpy(conn->subscriptions[i], subject, strlen(subject) + 1);
}

void connectionRemoveSubscription(Connection* conn, char* subject){
  int i = findEqual(conn->subscriptions, subject, conn->numSubs);
  if (i > 0){
    free(conn->subscriptions[i]);
    conn->subscriptions[i] = NULL;
  }
}

void connectionDestroy(Connection* conn){
  //int i;
  //printf("%p\n", conn->subscriptions);
  //for(i = 0; i<conn->numSubs; i++){
  //  printf("%p\n", conn->subscriptions[i]);
  //  if (conn->subscriptions[i] != NULL){
      //free(conn->subscriptions[i]);
  //  }
  //}
  //free(conn->subscriptions);

  char* path = malloc(strlen(conn->name) + 1 + 6);
  memcpy(path, "/tmp/", 6);
  strcat(path, conn->name);
  unlink(path);
  free(path);

  free(conn->name);
  free(conn);
}
