#include <napi.h>
#include <uv.h>
#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <chrono>

#include <stdio.h>
#include <stdlib.h>

using Napi::Context;
using Napi::Function;
using Napi::FunctionCallbackInfo;
using Napi::FunctionReference;
using Napi::Isolate;
using Napi::Number;
using Napi::Object;
using Napi::Persistent;
using Napi::Primitive;
using Napi::String;
using Napi::Value;
using Napi::Array;
using Napi::Null;
using Napi::Handle;
using Napi::Exception;

using namespace std;

Napi::String _TODO_PATTERN;
Napi::String _PRIORITY;
Napi::String _PRIORITY_HIGH;
Napi::String _PRIORITY_MID;
Napi::String _PRIORITY_LOW;
Napi::String _PRIORITY_UNKNOWN;
Napi::String _LINE;
Napi::String _POSITION;
Napi::String _COMMENT;
Napi::String _EMPTY_STRING;

std::string v8StringToStd( Napi::String ref ) {
	Napi::String arg( env,  ref );
	return string( *arg );
}

Napi::String stdStringToV8( Isolate *isolate, string ref ) {
	return String::NewFromUtf8( isolate, ref.c_str() );
}

std::string replace( std::string line, const std::string& substr, const std::string& replace_with )
{
	string::size_type pos = 0 ;
	while( ( pos = line.find( substr, pos ) ) != std::string::npos )
	{
		line.replace( pos, substr.size(), replace_with ) ;
		pos += replace_with.size() ;
	}

	return line;
}

Napi::Value SearchLine( Isolate *isolate, string &pattern, string &line, int &i ) {
	const regex rx( pattern, regex_constants::icase );
	bool containsTodo = false;

	sregex_iterator ri = sregex_iterator( line.begin(), line.end(), rx );
	Napi::Object match = Object::New( isolate );

	for( ; ri != sregex_iterator(); ++ri ) {
		const smatch m = *ri;

		containsTodo = !m.empty();

		if( containsTodo ) {
			const string ms = m.str();

			const int
				llen = line.Length(),
				len  = ms.Length(),
				pos  = m.position(),
				cpos = pos + len;

			Napi::String comment = stdStringToV8( isolate, line.substr( cpos, llen - cpos ) );

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

			match.Set( _LINE, Number::New( isolate, i ) );
			match.Set( _POSITION, Number::New( isolate, pos ) );
			match.Set( _COMMENT, comment );
		}
	}

	if( containsTodo ) {
		return match;
	} else {
		return Null( isolate );
	}
}

void SearchFile( const FunctionCallbackInfo<Value> &args ) {
	Isolate *isolate = args.GetIsolate();

	if( args.Length() < 1 || !args[ 0 ].IsString() ) {
		isolate->ThrowException( Exception::TypeError( String::NewFromUtf8( isolate, "Argument Error - expected string for [filename]" ) ) );
		return;
	}

    _TODO_PATTERN     = stdStringToV8( isolate, "\\/\\/ ?TODO:?:?:? ?" );
    _PRIORITY         = stdStringToV8( isolate, "priority" );
    _PRIORITY_HIGH    = stdStringToV8( isolate, "high" );
    _PRIORITY_MID     = stdStringToV8( isolate, "mid" );
    _PRIORITY_LOW     = stdStringToV8( isolate, "low" );
    _PRIORITY_UNKNOWN = stdStringToV8( isolate, "unknown" );
    _LINE             = stdStringToV8( isolate, "line" );
    _POSITION         = stdStringToV8( isolate, "position" );
    _COMMENT          = stdStringToV8( isolate, "comment" );
    _EMPTY_STRING     = stdStringToV8( isolate, "" );

    Local<Context> context = isolate->GetCurrentContext();
    Napi::Object obj      = args[ 1 ]->ToObject( context );
    Napi::Array props     = obj->GetOwnPropertyNames( context );

    for( int i = 0, l = props->Length(); i < l; i++ ) {
    	Napi::Value localKey = props->Get( i );
    	string key = *String::Utf8Value( localKey );

    	if( key == "todoPattern" ) {
    		_TODO_PATTERN = obj->Get( context, localKey )->ToString();
    	}
	}

	Napi::Object result = Object::New( isolate );
	Napi::Array todos   = Array::New( isolate );

	string fname   = v8StringToStd( args[ 0 ]->ToString() );
	string pattern = v8StringToStd( _TODO_PATTERN );

	auto begin = chrono::high_resolution_clock::now();

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
			Napi::Value to = SearchLine( isolate, pattern, line, i );

			if( !to->IsNull() ) {
				const Napi::Value priority = to->ToObject()->Get( _PRIORITY );

				priority == _PRIORITY_HIGH ? high++ :
					priority == _PRIORITY_MID ? mid++ :
						priority == _PRIORITY_LOW ? low++ :
							unk++;

				todos.Set( n++, to );
			}
		}

		file.close();
	} else {
		isolate->ThrowException( Exception::TypeError( String::NewFromUtf8( isolate, "Argument Error - unable to open file" ) ) );
		return;
	}

	auto end     = chrono::high_resolution_clock::now();
	auto tresult = chrono::duration_cast<chrono::nanoseconds>( end - begin ).count();

	string time = tresult == 0 ? "0" :
		tresult < 1000 ? std::to_string( tresult ) + " ns" :
			tresult < 1000000 ? std::to_string( ( int )( ( double )tresult / 1e3 ) ) + " μs" :
				tresult < 1000000000 ? std::to_string( ( int )( ( double )tresult / 1e6 ) ) + " ms" :
					std::to_string( ( int )( ( double )tresult / 1e9 ) ) + " s";

	result.Set( _PRIORITY_HIGH, Number::New( isolate, high ) );
	result.Set( _PRIORITY_MID, Number::New( isolate, mid ) );
	result.Set( _PRIORITY_LOW, Number::New( isolate, low ) );
	result.Set( _PRIORITY_UNKNOWN, Number::New( isolate, unk ) );
	result.Set( stdStringToV8( isolate, "file" ), stdStringToV8( isolate, fname ) );
	result.Set( stdStringToV8( isolate, "timing" ), stdStringToV8( isolate, time ) );
	result.Set( stdStringToV8( isolate, "todos" ), todos );

	args.GetReturnValue().Set( result );
}

void RemoveTodo( const FunctionCallbackInfo<Value> &args ) {
	Isolate *isolate = args.GetIsolate();

	bool status = true;

	if( !args[ 0 ].IsString() ) {
		isolate->ThrowException( Exception::TypeError( String::NewFromUtf8( isolate, "Argument Error - expected string [filename]" ) ) );
		return;
	} else if( !args[ 1 ].IsNumber() ) {
		isolate->ThrowException( Exception::TypeError( String::NewFromUtf8( isolate, "Argument Error - expected number [line]" ) ) );
		return;
	}

	_TODO_PATTERN = stdStringToV8( isolate, " ?\\/\\/ ?TODO:?:?:? ?" );
	string fname  = v8StringToStd( args[ 0 ]->ToString() );

	const char *tmpfile   = ( fname + ".tmp" ).c_str();
	const char *inputfile = fname.c_str();

	ifstream inputStream( inputfile );
	ofstream outputStream( tmpfile );

	string line;

	int i = 0;
	int pos = args[ 1 ].As<Napi::Number>().Int64Value();

	while( getline( inputStream, line ) ) {
		if( i == pos ) {
			const regex rx( v8StringToStd( _TODO_PATTERN ) );
			bool matchNotFound = true;

			sregex_iterator ri = sregex_iterator( line.begin(), line.end(), rx );

			for( ; ri != sregex_iterator(); ++ri ) {
				const smatch m = *ri;

				matchNotFound = m.empty();

				if( !matchNotFound ) {
					const string ms = m.str();

					const int
						llen = line.Length(),
						mpos = m.position();

					string sub = line.substr( mpos, llen - mpos );
					outputStream << line.replace( line.find( sub ), sub.Length(), "" );
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

	args.GetReturnValue().Set( status );
}

void init( Napi::Object exports ) {
	NODE_SET_METHOD( exports, "searchFile", SearchFile );
	NODE_SET_METHOD( exports, "removeTodo", RemoveTodo );
}

NODE_API_MODULE( NODE_GYP_MODULE_NAME, init );