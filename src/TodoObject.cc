#include "TodoObject.h"

Napi::FunctionReference TodoObject::constructor;

void TodoObject::Init( Napi::Env env ) {
	Napi::HandleScope scope( env );

	Napi::Function func = DefineClass( env, "TodoObject", {
//		InstanceMethod( "plusOne", &TodoObject::PlusOne ),
//		InstanceMethod( "value", &TodoObject::GetValue ),
//		InstanceMethod( "multiply", &TodoObject::Multiply ),
		InstanceValue( "stuff", Napi::Number::New( env, 30 ) )
	} );

	constructor = Napi::Persistent( func );
	constructor.SuppressDestruct();
}

Napi::Object TodoObject::New( Napi::Value arg ) {
	Napi::Object obj = constructor.New( { arg } );
	return obj;
}

TodoObject::TodoObject( const Napi::CallbackInfo& args ) : Napi::ObjectWrap< TodoObject >( args ) {
	Napi::Env env = args.Env();
	Napi::HandleScope scope( env );

	this->_filename = args[ 0 ].As< Napi::String >();
}



//Napi::Value TodoObject::GetValue( const Napi::CallbackInfo& info ) {
//	double num = this->value_;
//	return Napi::Number::New( info.Env(), num );
//}
//
//Napi::Value TodoObject::PlusOne( const Napi::CallbackInfo& info ) {
//	this->value_ = this->value_ + 1;
//
//	return TodoObject::GetValue( info );
//}
//
//Napi::Value TodoObject::Multiply( const Napi::CallbackInfo& info ) {
//	Napi::Number multiple;
//	if( info.Length() <= 0 || !info[ 0 ].IsNumber() ) {
//		multiple = Napi::Number::New( info.Env(), 1 );
//	} else {
//		multiple = info[ 0 ].As< Napi::Number >();
//	}
//
//	Napi::Object obj = constructor.New( { Napi::Number::New( info.Env(), this->value_ * multiple.DoubleValue() ) } );
//
//	return obj;
//}