#ifndef CONNECTION_H
#define CONNECTION_H

#include "ipc.h"

#define CONN_TYPE_ALL 1
#define CONN_TYPE_PID 2
#define CONN_TYPE_SUB 3

#define MAX_MSG_SIZE 1048576

typedef struct {
  char* name;
  int type;
} Connection;

typedef void (*ConnectionCallback)(Message* msg);

Connection* connectionCreate(char* name, int type);
Connection* connectionConnect(char* name, int type);
void connectionStartAutoDispatch(Connection* conn);
void connectionStopAutoDispatch(Connection* conn);
void connectionSetCallback(Connection* conn, ConnectionCallback cb);
ConnectionCallback connectionGetCallback(Connection* conn);
void connectionRemoveCallback(Connection* conn);
void connectionSend(Connection* conn, Message* msg);
void connectionDestroy(Connection* conn);

#endif
