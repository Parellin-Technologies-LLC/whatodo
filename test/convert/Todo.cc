#include "Todo.h"
#include <assert.h>

Todo::Todo() : env_( nullptr ), wrapper_( nullptr ) {}

Todo::~Todo() { napi_delete_reference( env_, wrapper_ ); }

void Todo::Destructor( napi_env env, void* nativeObject, void* /*finalize_hint*/ ) {
	reinterpret_cast< Todo* >( nativeObject )->~Todo();
}

napi_ref Todo::constructor;

napi_status Todo::Init( napi_env env ) {
	napi_status status;

	napi_value cons;
	status = napi_define_class( env, "Todo", NAPI_AUTO_LENGTH, New, nullptr, 0, nullptr, &cons );
	if( status != napi_ok ) return status;

	status = napi_create_reference( env, cons, 1, &constructor );
	if( status != napi_ok ) return status;

	return napi_ok;
}

napi_value Todo::New( napi_env env, napi_callback_info info ) {
	napi_status status;

	size_t argc = 1;
	napi_value args[ 1 ];
	napi_value _value;

	status = napi_get_cb_info( env, info, &argc, args, &_value, nullptr );
	assert( status == napi_ok );

	napi_valuetype valuetype;
	status = napi_typeof( env, args[ 0 ], &valuetype );
	assert( status == napi_ok );

	Todo *obj = new Todo();

	if( valuetype == napi_undefined ) {
		obj->fileName = nullptr;
	} else {
		std::string arg = args[ 0 ].As< String >().Utf8Value();

		char buffer[ 128 ];
		size_t buffer_size = 128;
		status = napi_get_value_string_utf8( env, args[ 0 ], nullptr, nullptr, &obj->fileName );

//		status = napi_get_value_double( env, args[ 0 ],  );
		assert( status == napi_ok );
	}

	obj->env_ = env;

	status = napi_wrap( env, _value, reinterpret_cast< void* >( obj ), Todo::Destructor, nullptr, &obj->wrapper_ );
	assert( status == napi_ok );

	return _value;
}

napi_status Todo::NewInstance( napi_env env, napi_value arg, napi_value* instance ) {
	napi_status status;

	const int argc = 1;
	napi_value argv[ argc ] = { arg };

	napi_value cons;
	status = napi_get_reference_value( env, constructor, &cons );
	if( status != napi_ok ) return status;

	status = napi_new_instance( env, cons, argc, argv, instance );
	if( status != napi_ok ) return status;

	return napi_ok;
}