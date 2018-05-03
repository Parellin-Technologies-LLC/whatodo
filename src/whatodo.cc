#include "whatodo.h"

using namespace Napi;
using namespace std;

String _TODO_PATTERN;
String _PRIORITY;
String _PRIORITY_HIGH;
String _PRIORITY_MID;
String _PRIORITY_LOW;
String _PRIORITY_UNKNOWN;
String _LINE;
String _POSITION;
String _COMMENT;
String _EMPTY_STRING;

//void RemoveTodo( const FunctionCallbackInfo<Value> &args ) {
//	Isolate *isolate = args.GetIsolate();
//
//	bool status = true;
//
//	if( !args[ 0 ].IsString() ) {
//		isolate->ThrowException( Exception::TypeError( String::NewFromUtf8( isolate, "Argument Error - expected string [filename]" ) ) );
//		return;
//	} else if( !args[ 1 ].IsNumber() ) {
//		isolate->ThrowException( Exception::TypeError( String::NewFromUtf8( isolate, "Argument Error - expected number [line]" ) ) );
//		return;
//	}
//
//	_TODO_PATTERN = stdStringToV8( isolate, " ?\\/\\/ ?TODO:?:?:? ?" );
//	string fname  = v8StringToStd( args[ 0 ]->ToString() );
//
//	const char *tmpfile   = ( fname + ".tmp" ).c_str();
//	const char *inputfile = fname.c_str();
//
//	ifstream inputStream( inputfile );
//	ofstream outputStream( tmpfile );
//
//	string line;
//
//	int i = 0;
//	int pos = args[ 1 ].As<Napi::Number>().Int64Value();
//
//	while( getline( inputStream, line ) ) {
//		if( i == pos ) {
//			const regex rx( v8StringToStd( _TODO_PATTERN ) );
//			bool matchNotFound = true;
//
//			sregex_iterator ri = sregex_iterator( line.begin(), line.end(), rx );
//
//			for( ; ri != sregex_iterator(); ++ri ) {
//				const smatch m = *ri;
//
//				matchNotFound = m.empty();
//
//				if( !matchNotFound ) {
//					const string ms = m.str();
//
//					const int
//						llen = line.Length(),
//						mpos = m.position();
//
//					string sub = line.substr( mpos, llen - mpos );
//					outputStream << line.replace( line.find( sub ), sub.Length(), "" );
//				}
//			}
//
//			if( matchNotFound ) {
//				outputStream << line;
//			}
//		} else {
//			outputStream << line << endl;
//		}
//
//		i++;
//	}
//
//	inputStream.close();
//	outputStream.close();
//
//	const char *ftmp = ( fname + ".tmp" ).c_str();
//
//	int result = rename( ftmp, inputfile );
//	if( result != 0 ) {
//		status = false;
//	}
//
//	args.GetReturnValue().Set( status );
//}

Value SearchLine( Env env, string &pattern, string &line, int &i ) {
	const regex rx( pattern, regex_constants::icase );
	bool containsTodo = false;

	sregex_iterator ri = sregex_iterator( line.begin(), line.end(), rx );
	Object match = Object::New( env );

	for( ; ri != sregex_iterator(); ++ri ) {
		const smatch m = *ri;

		containsTodo = !m.empty();

		if( containsTodo ) {
			const string ms = m.str();

			const int
				llen = line.length(),
				len  = ms.length(),
				pos  = m.position(),
				cpos = pos + len;

			String comment = String::New( env, line.substr( cpos, llen - cpos ) );

			if( comment == _EMPTY_STRING ) {
				containsTodo = false;
				break;
			}

			match.Set( _PRIORITY,
				ms.find( ":::" ) != string::npos ? _PRIORITY_HIGH :
					ms.find( "::" ) != string::npos ? _PRIORITY_MID :
						ms.find( ":" ) != string::npos ? _PRIORITY_LOW :
							_PRIORITY_UNKNOWN
			);

			match.Set( _LINE, Number::New( env, i ) );
			match.Set( _POSITION, Number::New( env, pos ) );
			match.Set( _COMMENT, comment );
		}
	}

	if( containsTodo ) {
		return match;
	} else {
		return env.Null();
//		return Null( env );
	}
}

Promise searchFile( const CallbackInfo &info ) {
	Env env = info.Env();
	TodoObject::Init( env );

	auto deferred = Promise::Deferred::New( env );

	if( info.Length() < 1 || !info[ 0 ].IsString() ) {
		deferred.Reject(
			TypeError::New( env, "Argument Error - expected string for [filename] parameter [ 0 ]" ).Value()
		);

		return deferred.Promise();
	} else if( !info[ 1 ].IsObject() ) {
		deferred.Reject(
			TypeError::New( env, "Argument Error - expected object for parameter [ 1 ]" ).Value()
		);

		return deferred.Promise();
	}

	string fname = info[ 0 ].ToString();
	Object obj   = info[ 1 ].ToObject();

	Name key      = String::New( env, "todoPattern" ).As< Name >();
	_TODO_PATTERN = obj.Get( static_cast< napi_value >( key ) ).As< String >();

	TodoObject *todo = ObjectWrap< TodoObject >::Unwrap( info[ 0 ].As< Object >() );
//	Object todo = TodoObject::New( info[ 0 ] );
	todo->setTodoPattern( _TODO_PATTERN );

	auto begin = chrono::high_resolution_clock::now();

//	Napi::Object result = Object::New( isolate );
//	Napi::Array todos   = Array::New( isolate );
//
//	string fname   = v8StringToStd( args[ 0 ]->ToString() );
	string pattern = _TODO_PATTERN.ToString();
//
//	auto begin = chrono::high_resolution_clock::now();
//
//	int i    = 0,
//		n    = 0,
//		high = 0,
//		mid  = 0,
//		low  = 0,
//		unk  = 0;
//
//	string line;
//	ifstream file( fname );
//
//	if( file.is_open() ) {
//		while( getline( file, line ) )
//		{
//			++i;
//			Value to = SearchLine( env, pattern, line, i );
//
//			if( !to->IsNull() ) {
//				const Value priority = to->ToObject()->Get( _PRIORITY );
//
//				priority == _PRIORITY_HIGH ? high++ :
//					priority == _PRIORITY_MID ? mid++ :
//						priority == _PRIORITY_LOW ? low++ :
//							unk++;
//
//				todos.Set( n++, to );
//			}
//		}
//
//		file.close();
//	} else {
//		deferred.Reject(
//			TypeError::New( env, "Argument Error - unable to open file" ).Value()
//		);
//
//		return deferred.Promise();
//	}
//
//	auto end     = chrono::high_resolution_clock::now();
//	auto tresult = chrono::duration_cast<chrono::nanoseconds>( end - begin ).count();
//
//	string time = tresult == 0 ? "0" :
//		tresult < 1000 ? std::to_string( tresult ) + " ns" :
//			tresult < 1000000 ? std::to_string( ( int )( ( double )tresult / 1e3 ) ) + " μs" :
//				tresult < 1000000000 ? std::to_string( ( int )( ( double )tresult / 1e6 ) ) + " ms" :
//					std::to_string( ( int )( ( double )tresult / 1e9 ) ) + " s";

	deferred.Resolve( todo.ToObject() );
//	deferred.Resolve( ObjectWrap< TodoObject >( todo ) );
//	deferred.Resolve( Object::New( env, todo ) );

//	result.Set( _PRIORITY_HIGH, Number::New( isolate, high ) );
//	result.Set( _PRIORITY_MID, Number::New( isolate, mid ) );
//	result.Set( _PRIORITY_LOW, Number::New( isolate, low ) );
//	result.Set( _PRIORITY_UNKNOWN, Number::New( isolate, unk ) );
//	result.Set( stdStringToV8( isolate, "file" ), stdStringToV8( isolate, fname ) );
//	result.Set( stdStringToV8( isolate, "timing" ), stdStringToV8( isolate, time ) );
//	result.Set( stdStringToV8( isolate, "todos" ), todos );

	return deferred.Promise();
}

Promise initialize( const CallbackInfo &info ) {
	Env env = info.Env();
	auto deferred = Promise::Deferred::New( env );

	deferred.Resolve( Boolean::New( env, true ) );

	return deferred.Promise();
}

Object init( Env env, Object exports ) {
	String
		name        = String::New( env, "name" ),
		_initialize = String::New( env, "initialize" ),
		_searchFile = String::New( env, "searchFile" );

	exports.Set( name, "whatodo" );
	exports.Set( _initialize, Function::New( env, initialize ) );
	exports.Set( _searchFile, Function::New( env, searchFile ) );

	_TODO_PATTERN     = String::New( env, "\\/\\/ ?TODO:?:?:? ?" );
	_PRIORITY         = String::New( env, "priority" );
	_PRIORITY_HIGH    = String::New( env, "high" );
	_PRIORITY_MID     = String::New( env, "mid" );
	_PRIORITY_LOW     = String::New( env, "low" );
	_PRIORITY_UNKNOWN = String::New( env, "unknown" );
	_LINE             = String::New( env, "line" );
	_POSITION         = String::New( env, "position" );
	_COMMENT          = String::New( env, "comment" );
	_EMPTY_STRING     = String::New( env, "" );

	return exports;
}

NODE_API_MODULE( NODE_GYP_MODULE_NAME, init );