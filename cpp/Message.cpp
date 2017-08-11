extern "C"{
 #include "ipc.h"
}
typedef Message IPC_Message;

#include "Message.h"


IPC::Message::Message(char* data, size_t len){
  ptr = messageCreate(data, len);
}

void IPC::Message::setPID(PID pid){
  messageSetPID((IPC_Message*) ptr, pid);
}

void IPC::Message::setSubject(char* subject){
  messageSetSubject((IPC_Message*) ptr, subject);
}

size_t IPC::Message::getLen(){
  return ((IPC_Message*) ptr)->len;
}

char* IPC::Message::getData(){
  return ((IPC_Message*) ptr)->data;
}

void* IPC::Message::getCPointer(){
  return ptr;
}

IPC::Message::~Message(){
  messageDestroy((IPC_Message*) ptr);
}
