#include "TodoObject.h"

Napi::FunctionReference TodoObject::constructor;

void TodoObject::Init( Napi::Env env ) {
//	Napi::HandleScope scope( env );

	Napi::Function func = DefineClass( env, "TodoObject", {
		InstanceAccessor( "filename", &TodoObject::getFilename, nullptr ),
		InstanceAccessor( "todoPattern", &TodoObject::getTodoPattern, nullptr )

//		InstanceMethod( "getFilename", &TodoObject::getFilename ),
//		InstanceMethod( "getTodoPattern", &TodoObject::getTodoPattern )
	} );

	constructor = Napi::Persistent( func );
	constructor.SuppressDestruct();
}

Napi::Object TodoObject::New( Napi::Value arg ) {
	Napi::Object obj = constructor.New( { arg } );
	return obj;
}

TodoObject::TodoObject( const Napi::CallbackInfo &args ) : Napi::ObjectWrap< TodoObject >( args ) {
	Napi::Env env = args.Env();
//	Napi::HandleScope scope( env );

	this->_filename = args[ 0 ].As< Napi::String >();
}

Array GetPropertyNames( const CallbackInfo& info ) {
	Object obj = info[ 0 ].As< Object >();
	Array arr = obj.GetPropertyNames();
	return arr;
}

Napi::Value TodoObject::ToObject( Napi::Env env, const TodoObject *tobj ) {
	Napi::Object obj = Object::New( env );
	return GetPropertyNames( tobj );
}

Napi::Value TodoObject::getFilename( const Napi::CallbackInfo &info ) {
	return Napi::String::New( info.Env(), this->_filename );
}

Napi::Value TodoObject::getTodoPattern( const Napi::CallbackInfo &info ) {
	return Napi::String::New( info.Env(), this->_todo_pattern );
}

void setTodoPattern( const Napi::Value& n ) {
	_todo_pattern = n.As< Napi::String >();
}


//Napi::Value TodoObject::PlusOne( const Napi::CallbackInfo &info ) {
//	this->value_ = this->value_ + 1;
//	return TodoObject::GetValue( info );
//}
//Napi::Value TodoObject::Multiply( const Napi::CallbackInfo &info ) {
//	Napi::Number multiple;
//	if( info.Length() <= 0 || !info[ 0 ].IsNumber() ) {
//		multiple = Napi::Number::New( info.Env(), 1 );
//	} else {
//		multiple = info[ 0 ].As< Napi::Number >();
//	}
//	Napi::Object obj = constructor.New( { Napi::Number::New( info.Env(), this->value_ * multiple.DoubleValue() ) } );
//	return obj;
//}