#ifndef MYOBJECT_H
#define MYOBJECT_H

#include <node.h>
//#include "RCSwitch.h"

class MyObject : public node::ObjectWrap {
public:
  static void Init(v8::Handle<v8::Object> exports);

private:
  MyObject();
  ~MyObject();

  static v8::Handle<v8::Value> New(const v8::Arguments& args);
  static v8::Handle<v8::Value> Start(const v8::Arguments& args);
  static void RCHandleThread(uv_work_t *req);
  static void RCHandleReceived(uv_async_t *handle, int status);

  static v8::Persistent<v8::Function> constructor;
  //static RCSwitch radio;
  static uv_loop_t *loop;
  static uv_async_t async;

};

#endif
