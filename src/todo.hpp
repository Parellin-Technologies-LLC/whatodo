#include <node.h>
#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <chrono>

#include <stdio.h>
#include <stdlib.h>

#include "string_utils.cpp"

using v8::Context;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::Isolate;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::Persistent;
using v8::Primitive;
using v8::String;
using v8::Value;
using v8::Array;
using v8::Null;
using v8::Handle;
using v8::Exception;

using namespace std;

Local<String> _TODO_PATTERN;
Local<String> _PRIORITY;
Local<String> _PRIORITY_HIGH;
Local<String> _PRIORITY_MID;
Local<String> _PRIORITY_LOW;
Local<String> _PRIORITY_UNKNOWN;
Local<String> _LINE;
Local<String> _POSITION;
Local<String> _COMMENT;
Local<String> _EMPTY_STRING;