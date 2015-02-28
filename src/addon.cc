#define BUILDING_NODE_EXTENSION
#include <node.h>
#include "RCSwitch.h"

using namespace v8;

uv_async_t async;
RCSwitch radio;
Persistent<Function> callback;

void handleReceived(uv_async_t *handle, int status) {
  if (radio.available()) {
    if (radio.getReceivedBitlength() == 24) {
      unsigned int value = radio.getReceivedValue();
      unsigned int identifier = value >> 16;
      float temperature = static_cast<float>((value & 0xFFFF) / 16.0);

      Handle<Value> argv[2] = {
        Number::New(identifier),
        Number::New(temperature)
      };
 
      TryCatch try_catch;

      node::MakeCallback(Context::GetCurrent()->Global(), callback, 2, argv);

      if (try_catch.HasCaught()) {
        node::FatalException(try_catch);
      }
    }
    radio.resetAvailable();
  } 
}

Handle<Value> LightOff(const Arguments& args) {
  HandleScope scope;

  assert(args.Length() == 0);

  digitalWrite(0, LOW);

  return scope.Close(True());
}

Handle<Value> LightOn(const Arguments& args) {
  HandleScope scope;

  assert(args.Length() == 0);

  digitalWrite(0, HIGH);

  return scope.Close(True());
}

Handle<Value> Start(const Arguments& args) {
  HandleScope scope;

  assert(args.Length() == 2);
  assert(args[0]->IsNumber());
  assert(args[1]->IsFunction());

  int PIN = args[0]->NumberValue();
  callback = Persistent<Function>::New(args[1].As<Function>());

  wiringPiSetup();
  pinMode(0, OUTPUT);

  uv_async_init(uv_default_loop(), &async, handleReceived);
  
  radio = RCSwitch();
  radio.enableReceive(PIN, &async);

  return Undefined();
}

void Init(Handle<Object> exports) {
 exports->Set(String::NewSymbol("start"),
      FunctionTemplate::New(Start)->GetFunction());
 exports->Set(String::NewSymbol("lightOn"),
      FunctionTemplate::New(LightOn)->GetFunction());
 exports->Set(String::NewSymbol("lightOff"),
      FunctionTemplate::New(LightOff)->GetFunction());
 
}

NODE_MODULE(addon, Init)
