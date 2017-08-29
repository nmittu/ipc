#include "ConnectionWrap.h"
#include "MessageWrap.h"
extern "C"{
	#include "hashtable.h"
}

using namespace std;
using namespace v8;
using namespace node;


#define HT_CAPACITY 50

hashtable_t* ht;

void init(){
	ht = ht_create(HT_CAPACITY);
}


void cppCallback(IPC::Message msg){
  char* name = msg.getData() + msg.getLen() + sizeof(size_t);

	Nan::Callback* cb = (Nan::Callback*) ht_get(ht, name);

	Local<Function> cons = Nan::New(MessageWrap::constructor());

	uint64_t ptr = (uint64_t) msg.getCPointer();
	uint32_t ptr_hi = ptr >> 32;
	uint32_t ptr_lo = ptr & 0xffffffff;

	Local<Value> cons_argv[2] = {Nan::New<Uint32>(ptr_hi), Nan::New<Uint32>(ptr_lo)};
	Local<Value> argv[1] = {Nan::NewInstance(cons, 2, cons_argv).ToLocalChecked()};

	cb->Call(1, argv);
}


NAN_MODULE_INIT(ConnectionWrap::Init){
	init();
  //TODO
}

ConnectionWrap::ConnectionWrap(char* name, int type, int create){
  conn = new IPC::Connection(name, type, create);
}

ConnectionWrap::~ConnectionWrap(){
  delete conn;
}

NAN_METHOD(ConnectionWrap::New){
  std::string name(*Nan::Utf8String(info[0].As<String>()));
  int type = info[1]->IntegerValue();
  int create = 1;
  if (info.Length() == 3){
    create = info[2]->BooleanValue();
  }

  ConnectionWrap* obj = new ConnectionWrap((char*) name.c_str(), type, create);
  obj->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(ConnectionWrap::startAutoDispatch){
  IPC::Connection* conn = Nan::ObjectWrap::Unwrap<ConnectionWrap>(info.Holder())->conn;
  conn->startAutoDispatch();
}

NAN_METHOD(ConnectionWrap::stopAutoDispatch){
  IPC::Connection* conn = Nan::ObjectWrap::Unwrap<ConnectionWrap>(info.Holder())->conn;
  conn->stopAutoDispatch();
}

NAN_METHOD(ConnectionWrap::setCallback){
	IPC::Connection* conn = Nan::ObjectWrap::Unwrap<ConnectionWrap>(info.Holder())->conn;

	Nan::Callback* cb = new Nan::Callback(Local<Function>::Cast(info[0]));
	ht_put(ht, conn->getName(), cb);

	conn->setCallback(cppCallback);
}

NAN_METHOD(ConnectionWrap::getCallback){
	IPC::Connection* conn = Nan::ObjectWrap::Unwrap<ConnectionWrap>(info.Holder())->conn;

	Nan::Callback* cb = (Nan::Callback*)ht_get(ht, conn->getName());
	info.GetReturnValue().Set(cb->GetFunction());
}

NAN_METHOD(ConnectionWrap::removeCallback){
	IPC::Connection* conn = Nan::ObjectWrap::Unwrap<ConnectionWrap>(info.Holder())->conn;

	conn->removeCallback();
	delete (Nan::Callback*)ht_remove(ht, conn->getName());
}

NAN_METHOD(ConnectionWrap::send){
	IPC::Connection* conn = Nan::ObjectWrap::Unwrap<ConnectionWrap>(info.Holder())->conn;

	IPC::Message msg = *(Nan::ObjectWrap::Unwrap<MessageWrap>(Nan::To<Object>(info[0]).ToLocalChecked())->msg);
	conn->send(msg);
}

NAN_METHOD(ConnectionWrap::subscribe){
	IPC::Connection* conn = Nan::ObjectWrap::Unwrap<ConnectionWrap>(info.Holder())->conn;

	std::string subject(*Nan::Utf8String(info[0].As<String>()));
	conn->subscribe((char*) subject.c_str());
}

NAN_METHOD(ConnectionWrap::removeSubscription){
	IPC::Connection* conn = Nan::ObjectWrap::Unwrap<ConnectionWrap>(info.Holder())->conn;

	std::string subject(*Nan::Utf8String(info[0].As<String>()));
	conn->removeSubscription((char*) subject.c_str());
}

NAN_METHOD(ConnectionWrap::close){
	IPC::Connection* conn = Nan::ObjectWrap::Unwrap<ConnectionWrap>(info.Holder())->conn;
	conn->close();
}

inline Nan::Persistent<v8::Function> & ConnectionWrap::constructor(){
	static Nan::Persistent<v8::Function> my_constructor;
  return my_constructor;
}
