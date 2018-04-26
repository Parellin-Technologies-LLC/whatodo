#include "whatodo.h"

napi_deferred deferredd = nullptr;

napi_value searchFile( napi_env env, napi_callback_info info ) {
	napi_deferred deferred;
    napi_value promise;
    napi_status status;

    size_t argc = 1;
    napi_value args[ 1 ];

    NAPI_CALL( env, napi_get_cb_info( env, info, &argc, args, nullptr, nullptr ) );

    NAPI_ASSERT( env, argc >= 1, "Wrong number of arguments" );

    napi_valuetype valuetype;
    NAPI_CALL( env, napi_typeof( env, args[ 0 ], &valuetype ) );

    NAPI_ASSERT( env, valuetype == napi_string, "Argument Error - Expect typeof string");

//    char buffer[ 128 ];
//    size_t buffer_size = 128;
//    NAPI_CALL( env, napi_get_value_string_utf8( env, args[ 0 ], buffer, buffer_size, nullptr ) );
//	cout << buffer;

    napi_value instance;
    status = Todo::NewInstance( env, args[ 0 ], &instance );

//    Todo *_todo;
//    status = napi_unwrap( env, args[ 0 ], reinterpret_cast< void** >( &_todo ) );
    assert( status == napi_ok );

    NAPI_CALL( env, napi_create_promise( env, &deferred, &promise ) );

    return promise;
}


napi_value createPromise( napi_env env, napi_callback_info info ) {
	napi_value promise;

	if( deferredd != nullptr ) {
		return nullptr;
	}

	NAPI_CALL( env, napi_create_promise( env, &deferredd, &promise ) );

	return promise;
}

napi_value concludeCurrentPromise( napi_env env, napi_callback_info info ) {
	if( deferredd == nullptr ) {
		return nullptr;
    }

	size_t argc = 2;
	napi_value args[ 2 ];

	bool resolution;µ

	NAPI_CALL( env, napi_get_cb_info( env, info, &argc, args, nullptr, nullptr ) );

	NAPI_CALL( env, napi_get_value_bool( env, args[ 1 ], &resolution ) );

	if( resolution ) {
		NAPI_CALL( env, napi_resolve_deferred( env, deferredd, args[ 0 ] ) );
	} else {
		NAPI_CALL( env, napi_reject_deferred( env, deferredd, args[ 0 ] ) );
	}

	deferredd = nullptr;

	return nullptr;
}

napi_value isPromise( napi_env env, napi_callback_info info ) {
	napi_value promise, result;
	size_t argc = 1;
	bool is_promise;

	NAPI_CALL( env, napi_get_cb_info( env, info, &argc, &promise, nullptr, nullptr ) );
	NAPI_CALL( env, napi_is_promise( env, promise, &is_promise ) );
	NAPI_CALL( env, napi_get_boolean( env, is_promise, &result ) );

	return result;
}

napi_value init( napi_env env, napi_value exports ) {
	napi_status status;

//	napi_value fn;
//	status = napi_create_function( env, "", NAPI_AUTO_LENGTH, searchFile, nullptr, &fn );
//	assert( status == napi_ok );
//	status = napi_set_named_property( env, exports, "searchFile", fn );
//	assert( status == napi_ok );

	napi_property_descriptor descriptors[] = {
		DECLARE_NAPI_PROPERTY( "createPromise", createPromise ),
		DECLARE_NAPI_PROPERTY( "concludeCurrentPromise", concludeCurrentPromise ),
		DECLARE_NAPI_PROPERTY( "isPromise", isPromise ),
		DECLARE_NAPI_PROPERTY( "searchFile", searchFile )
	};

	NAPI_CALL( env,
		napi_define_properties(
			env,
			exports,
			sizeof( descriptors ) / sizeof( *descriptors ),
			descriptors
		)
	);

	return exports;
}

NAPI_MODULE( NODE_GYP_MODULE_NAME, init );