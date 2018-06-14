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

Promise removeTodo( const CallbackInfo &info ) {
	Env env       = info.Env();
	bool status   = true;
	auto deferred = Promise::Deferred::New( env );

	if( info.Length() < 1 || !info[ 0 ].IsString() ) {
		deferred.Reject(
			TypeError::New( env, "Argument Error - expected string for [filename] parameter [ 0 ]" ).Value()
		);

		return deferred.Promise();
	} else if( !info[ 1 ].IsNumber() ) {
		deferred.Reject(
			TypeError::New( env, "Argument Error - expected number for parameter [ 1 ]" ).Value()
		);

		return deferred.Promise();
	}

	_TODO_PATTERN = String::New( env, " ?\\/\\/ ?TODO:?:?:? ?" );
	string fname  = info[ 0 ].ToString();

	const char *tmpfile   = ( fname + ".tmp" ).c_str();
	const char *inputfile = fname.c_str();

	ifstream inputStream( inputfile );
	ofstream outputStream( tmpfile );

	string line;

	int i = 0;
	int pos = info[ 1 ].As< Napi::Number >().Int64Value();

	while( getline( inputStream, line ) ) {
		if( i == pos ) {
			const regex rx( _TODO_PATTERN.Utf8Value() );
			bool matchNotFound = true;

			sregex_iterator ri = sregex_iterator( line.begin(), line.end(), rx );

			for( ; ri != sregex_iterator(); ++ri ) {
				const smatch m = *ri;

				matchNotFound = m.empty();

				if( !matchNotFound ) {
					const string ms = m.str();

					const int
						llen = line.length(),
						mpos = m.position();

					string sub = line.substr( mpos, llen - mpos );
					outputStream << line.replace( line.find( sub ), sub.length(), "" );
				}
			}

			if( matchNotFound ) {
				outputStream << line;
			}
		} else {
			outputStream << line << endl;
		}

		i++;
	}

	inputStream.close();
	outputStream.close();

	const char *ftmp = ( fname + ".tmp" ).c_str();

	int result = rename( ftmp, inputfile );
	if( result != 0 ) {
		status = false;
	}

	deferred.Resolve( Boolean::New( env, status ) );
	return deferred.Promise();
}

Value searchLine( Env env, string &pattern, string &line, int &i ) {
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

			match.Set( String::New( env, "priority" ),
				ms.find( ":::" ) != string::npos ? String::New( env, "high" ) :
					ms.find( "::" ) != string::npos ? String::New( env, "mid" ) :
						ms.find( ":" ) != string::npos ? String::New( env, "low" ) :
							String::New( env, "unknown" )
			);

			match.Set( String::New( env, "line" ), Number::New( env, i ) );
			match.Set( String::New( env, "position" ), Number::New( env, pos ) );
			match.Set( String::New( env, "comment" ), comment );
		}
	}

	if( containsTodo ) {
		return match;
	} else {
		return env.Null();
	}
}

Promise searchFile( const CallbackInfo &info ) {
	Env env = info.Env();

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

	Name key       = String::New( env, "todoPattern" ).As< Name >();
	_TODO_PATTERN  = obj.Get( static_cast< napi_value >( key ) ).As< String >();
	string pattern = _TODO_PATTERN.ToString();

	auto begin = chrono::high_resolution_clock::now();

	Object result = Object::New( env );
	Array todos = Array::New( env );

	int i    = 0,
		n    = 0,
		high = 0,
		mid  = 0,
		low  = 0,
		unk  = 0;

	string line;
	ifstream file( fname );

	if( file.is_open() ) {
		while( getline( file, line ) )
		{
			++i;
			Value to = searchLine( env, pattern, line, i );

			if( !to.IsNull() ) {
				const Value priority = to.ToObject().Get( String::New( env, "priority" ) );

				priority == String::New( env, "high" ) ? high++ :
					priority == String::New( env, "mid" ) ? mid++ :
						priority == String::New( env, "low" ) ? low++ :
							unk++;

				todos.Set( n++, to );
			}
		}

		file.close();
	} else {
		deferred.Reject(
			TypeError::New( env, "Argument Error - unable to open file" ).Value()
		);

		return deferred.Promise();
	}

	auto end     = chrono::high_resolution_clock::now();
	auto tresult = chrono::duration_cast<chrono::nanoseconds>( end - begin ).count();

	string time = tresult == 0 ? "0" :
		tresult < 1000 ? std::to_string( tresult ) + " ns" :
			tresult < 1000000 ? std::to_string( ( int )( ( double )tresult / 1e3 ) ) + " μs" :
				tresult < 1000000000 ? std::to_string( ( int )( ( double )tresult / 1e6 ) ) + " ms" :
					std::to_string( ( int )( ( double )tresult / 1e9 ) ) + " s";

	result.Set( String::New( env, "file" ), fname );
    result.Set( String::New( env, "todoPattern" ), pattern );
	result.Set( String::New( env, "high" ), Number::New( env, high ) );
	result.Set( String::New( env, "mid" ), Number::New( env, mid ) );
	result.Set( String::New( env, "low" ), Number::New( env, low ) );
	result.Set( String::New( env, "unknown" ), Number::New( env, unk ) );
	result.Set( String::New( env, "timing" ), String::New( env, time ) );
	result.Set( String::New( env, "todos" ), todos );

	deferred.Resolve( result );

	return deferred.Promise();
}

Promise initialize( const CallbackInfo &info ) {
	Env env = info.Env();
	auto deferred = Promise::Deferred::New( env );

	deferred.Resolve( Boolean::New( env, true ) );

	return deferred.Promise();
}

Object init( Env env, Object exports ) {
	exports.Set( String::New( env, "name" ), "whatodo" );
	exports.Set( String::New( env, "initialize" ), Function::New( env, initialize ) );
	exports.Set( String::New( env, "searchFile" ), Function::New( env, searchFile ) );
	exports.Set( String::New( env, "removeTodo" ), Function::New( env, removeTodo ) );

	_TODO_PATTERN     = String::New( env, "\\/\\/ ?TODO:?:?:? ?" );
	_EMPTY_STRING     = String::New( env, "" );

	return exports;
}

NODE_API_MODULE( NODE_GYP_MODULE_NAME, init );