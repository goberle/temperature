#define BUILDING_NODE_EXTENSION
#include <node.h>
#include "radio.h"
//#include "RCSwitch.h"

using namespace v8;
using namespace node;

static Persistent<String> emit_symbol = NODE_PSYMBOL("emit");

Persistent<Function> MyObject::constructor;
//RCSwitch MyObject::radio;

uv_loop_t MyObject::*loop;
uv_async_t MyObject::async;

MyObject::MyObject() {}
MyObject::~MyObject() {}

void MyObject::Init(Handle<Object> exports) {
  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
  tpl->SetClassName(String::NewSymbol("MyObject"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->PrototypeTemplate()->Set(
      String::NewSymbol("start"),
      FunctionTemplate::New(Start)->GetFunction()
  );
  exports->Set(String::NewSymbol("MyObject"), tpl->GetFunction());
}

Handle<Value> MyObject::New(const Arguments& args) {
  HandleScope scope;

  String::Utf8Value name(args[0]);
  MyObject* obj = new MyObject();
  obj->Wrap(args.This() );

  return args.This();
}

Handle<Value> MyObject::Start(const Arguments& args) {
  HandleScope scope;
  MyObject::loop = uv_default_loop();
  uv_work_t req;

  MyObject* obj = ObjectWrap::Unwrap<MyObject>(args.This());
  req.data = (void*) obj;
  
  uv_async_init(MyObject::loop, &MyObject::async, RCHandleReceived);
  uv_queue_work(MyObject::loop, &req, MyObject::RCHandleThread, NULL);

  return scope.Close(Undefined());
}

void MyObject::RCHandleThread(uv_work_t *req) {
  MyObject* obj = (MyObject*) req->data;
  //int PIN = 12;
  
  //wiringPiSetup();

  //MyObject::radio = RCSwitch();
  //auto fp = std::bind(&MyObject::RCHandleReceived, obj);
  //MyObject::radio.enableReceive(PIN, fp);
  //MyObject::radio.enableReceive(PIN);

  while (1) {
    //if (MyObject::radio.available()) {
      //if (MyObject::radio.getReceivedBitlength() == 24) {
        //unsigned int value = MyObject::radio.getReceivedValue();
        
        //unsigned int identifier = value >> 16;
        //float temperature = (float)(value & 0xFFFF) / 16.0;

        MyObject::async.data = (void*) obj;
        uv_async_send(&MyObject::async);

        sleep(1);
      //}
      //MyObject::radio.resetAvailable();
    //}
  }

}

void MyObject::RCHandleReceived(uv_async_t *handle, int status) {
  MyObject* obj = (MyObject*) handle->data;

  Local<Value> emit_v = obj->handle_->Get(emit_symbol);
  assert(emit_v->IsFunction());
  Local<Function> emit = emit_v.As<Function>();

  //if (test > 0) {
    Handle<Value> argv[3] = {
      String::New("data"),
      Number::New(1),
      Number::New(1)
    };

    TryCatch tc;
    emit->Call(obj->handle_, 3, argv);
    if (tc.HasCaught()) {
      FatalException(tc);
    }

  //} else {
  //  Handle<Value> argv[1] = {
  //    String::New("error"),
  //  };

  //  TryCatch tc;
  //  emit->Call(obj->handle_, 1, argv);
  //  if (tc.HasCaught()) {
  //    FatalException(tc);
  //  }
  //}
}
