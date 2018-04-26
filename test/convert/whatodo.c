//#include "napi.h"
#include <napi.h>

#include <assert.h>
#include "common.c"

#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <chrono>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

using namespace std;

napi_deferred deferredd = NULL;
//usleep( 5000000 );


napi_value searchFile( napi_env env, napi_callback_info info ) {
	napi_deferred deferred;
    napi_value promise;
    napi_status status;

    size_t argc = 1;
    napi_value argv[ 1 ];
    napi_value fileName;

    NAPI_CALL( env, napi_get_cb_info( env, info, &argc, argv, &fileName, nullptr ) );

//    string str;
//    status = napi_coerce_to_string( env, fileName, &str ) );

    cout << fileName;

    NAPI_CALL( env, napi_create_promise( env, &deferred, &promise ) );

    return promise;
}


napi_value createPromise( napi_env env, napi_callback_info info ) {
	napi_value promise;

	if( deferredd != NULL ) {
		return NULL;
	}

	NAPI_CALL( env, napi_create_promise( env, &deferredd, &promise ) );

	return promise;
}

napi_value concludeCurrentPromise( napi_env env, napi_callback_info info ) {
	size_t argc = 2;
	napi_value argv[ 2 ];

	bool resolution;

	NAPI_CALL( env, napi_get_cb_info( env, info, &argc, argv, NULL, NULL ) );

	NAPI_CALL( env, napi_get_value_bool( env, argv[ 1 ], &resolution ) );

	if( resolution ) {
		NAPI_CALL( env, napi_resolve_deferred( env, deferredd, argv[ 0 ] ) );
	} else {
		NAPI_CALL( env, napi_reject_deferred( env, deferredd, argv[ 0 ] ) );
	}

	deferredd = NULL;

	return NULL;
}

napi_value isPromise( napi_env env, napi_callback_info info ) {
	napi_value promise, result;
	size_t argc = 1;
	bool is_promise;

	NAPI_CALL( env, napi_get_cb_info( env, info, &argc, &promise, NULL, NULL ) );
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