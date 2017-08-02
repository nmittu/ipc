#ifndef MESSAGE_H
#define MESSAGE_H

#include <sys/types.h>


typedef struct {
  int type;
  union {
    pid_t pid;
    char* subject;
  };

  char* data;
  size_t len;
} Message;

#include "ipc.h"

Message* messageCreate(char* data, size_t len);
void messageSetPID(Message* msg, pid_t pid);
void messageSetSubject(Message* msg, char* subject);
void messageDestroy(Message* msg);

#endif
