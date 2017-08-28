#include <node.h>
#include "MessageWrap.h"

using namespace v8;

void InitAll(Handle<Object> exports) {
  MessageWrap::Init(exports);
}

NODE_MODULE(IPC, InitAll)
