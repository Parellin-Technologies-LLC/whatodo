#ifndef TODOOBJECT_H
#define TODOOBJECT_H

#include <napi.h>

class TodoObject : public Napi::ObjectWrap< TodoObject > {
	public:
		static Napi::Object Init( Napi::Env env, Napi::Object exports );
		TodoObject( const Napi::CallbackInfo& info );

	private:
		static Napi::FunctionReference constructor;

		Napi::Value GetValue( const Napi::CallbackInfo& info );
		Napi::Value PlusOne( const Napi::CallbackInfo& info );
		Napi::Value Multiply( const Napi::CallbackInfo& info );

		Napi::Number _high;
		Napi::Number _mid;
		Napi::Number _low;
		Napi::Number _unknown;
};

#endif