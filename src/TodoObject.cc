#include "TodoObject.h"

Napi::FunctionReference TodoObject::constructor;

Napi::Object TodoObject::Init( Napi::Env env, Napi::Object exports ) {
	Napi::HandleScope scope( env );

	Napi::Function func = DefineClass( env, "TodoObject", {
		InstanceMethod( "plusOne", &TodoObject::PlusOne ),
		InstanceMethod( "value", &TodoObject::GetValue ),
		InstanceMethod( "multiply", &TodoObject::Multiply )
	} );

	constructor = Napi::Persistent( func );
	constructor.SuppressDestruct();

	exports.Set( "TodoObject", func );

	return exports;
}

TodoObject::TodoObject( const Napi::CallbackInfo& info ) : Napi::ObjectWrap< TodoObject >( info ) {
	Napi::Env env = info.Env();
	Napi::HandleScope scope( env );

	int length = info.Length();

	if( length <= 0 || !info[ 0 ].IsNumber() ) {
		Napi::TypeError::New( env, "Number expected" ).ThrowAsJavaScriptException();
	}

	Napi::Number value = info[ 0 ].As< Napi::Number >();
	this->value_ = value.DoubleValue();
}

Napi::Value TodoObject::GetValue( const Napi::CallbackInfo& info ) {
	double num = this->value_;
	return Napi::Number::New( info.Env(), num );
}

Napi::Value TodoObject::PlusOne( const Napi::CallbackInfo& info ) {
	this->value_ = this->value_ + 1;

	return TodoObject::GetValue( info );
}

Napi::Value TodoObject::Multiply( const Napi::CallbackInfo& info ) {
	Napi::Number multiple;
	if( info.Length() <= 0 || !info[ 0 ].IsNumber() ) {
		multiple = Napi::Number::New( info.Env(), 1 );
	} else {
		multiple = info[ 0 ].As< Napi::Number >();
	}

	Napi::Object obj = constructor.New( { Napi::Number::New( info.Env(), this->value_ * multiple.DoubleValue() ) } );

	return obj;
}