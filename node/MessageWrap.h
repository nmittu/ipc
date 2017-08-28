#ifndef MESSAGE_WRAP_H
#define MESSAGE_WRAP_H

#include "ipc_cpp.h"
#include <nan.h>


class MessageWrap: public Nan::ObjectWrap {
public:
  static NAN_MODULE_INIT(Init);

private:
  char* data;
  IPC::Message* msg;

  MessageWrap(char* data, size_t len);
  ~MessageWrap();

  static NAN_METHOD(New);

  static NAN_METHOD(setPID);

  static NAN_METHOD(setSubject);

  static NAN_METHOD(getData);

  static inline Nan::Persistent<v8::Function> & constructor();
};

#endif
