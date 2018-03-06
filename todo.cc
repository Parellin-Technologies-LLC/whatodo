#include <node.h>
#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <chrono>

using v8::Context;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::Isolate;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::Persistent;
using v8::String;
using v8::Value;
using v8::Array;
using v8::Null;
using v8::Handle;
using v8::Exception;

using namespace std;

std::string string_format( const std::string fmt, ... ) {
    std::vector< char > str( 100, '\0' );
    va_list ap;

    while( 1 ) {
        va_start( ap, fmt );

        auto n = vsnprintf( str.data(), str.size(), fmt.c_str(), ap );

        va_end( ap );

        if( ( n > -1 ) && ( size_t( n ) < str.size() ) ) {
            return str.data();
        }

        if( n > -1 ) {
            str.resize( n + 1 );
		} else {
            str.resize( str.size() * 2 );
		}
    }

    return str.data();
}

std::string v8StringToStd( Local<String> ref ) {
	String::Utf8Value arg( ref );
	return std::string( *arg );
}

Local<String> stdStringToV8( Isolate *isolate, std::string ref ) {
	return String::NewFromUtf8( isolate, ref.c_str() );
}

//// TODO: allow regex override
Local<Value> SearchLine( Isolate *isolate, string &line, int &i ) {
    const std::regex rx( "\\/\\/ ?TODO:?:?:? ?" );
    bool matchFound;

    std::sregex_iterator ri = std::sregex_iterator( line.begin(), line.end(), rx );
    Local<Object> match = Object::New( isolate );

    for( ; ri != std::sregex_iterator(); ++ri ) {
        const std::smatch m = *ri;

        matchFound = m.empty();

        if( !matchFound ) {
            const string ms = m.str();

            const int
                llen = line.length(),
                len  = ms.length(),
                pos  = m.position(),
                cpos = pos + len;

            const string comment = line.substr( cpos, llen - cpos );

            if( comment == "" ) {
                break;
            }

//            match->Set( String::NewFromUtf8( isolate, "priority" ),
//                ms.find( ":::" ) != std::string::npos ? "HIGH" :
//                    ms.find( "::" ) != std::string::npos ? "MID" :
//                        ms.find( ":" ) != std::string::npos ? "LOW" :
//                            "UNKNOWN"
//            );

//            match->Set( stdStringToV8( isolate, "line" ), i );
//            match->Set( String::NewFromUtf8( isolate, "position" ), pos );
//            match->Set( String::NewFromUtf8( isolate, "comment" ), comment );
        }
    }

    return !matchFound ? match : Null( isolate );
}

void SearchFile( const FunctionCallbackInfo<Value> &args ) {
    Isolate *isolate = args.GetIsolate();

    if( args.Length() < 1 || !args[ 0 ]->IsString() ) {
    	isolate->ThrowException( Exception::TypeError( String::NewFromUtf8( isolate, "Argument Error - expected string" ) ) );
    	return;
    }

    std::string fname = v8StringToStd( args[ 0 ]->ToString() );

    Local<Object> result = Object::New( isolate );
    Local<Array> todos = Array::New( isolate );

    auto begin = std::chrono::high_resolution_clock::now();

    int i = 0,
        n = 0;

    string line;
    ifstream file( fname );

    if( file.is_open() ) {
        while( getline( file, line ) )
        {
            ++i;
            Local<Value> to = SearchLine( isolate, line, i );

            if( !to.IsEmpty() ) {
                todos->Set( n++, to );
            }
        }

        file.close();
    } else {
    	isolate->ThrowException( Exception::TypeError( String::NewFromUtf8( isolate, "Argument Error - unable to open file" ) ) );
        return;
    }

    auto end     = std::chrono::high_resolution_clock::now();
    auto tresult = std::chrono::duration_cast<std::chrono::nanoseconds>( end - begin ).count();

    std::string time = tresult == 0 ? "0" :
    	tresult < 1000 ? string_format( "%lld ns", tresult ) :
    		tresult < 1000000 ? string_format( "%.3f μs", ( double )tresult / 1e3 ) :
    			tresult < 1000000000 ? string_format( "%.3f ms", ( double )tresult / 1e6 ) :
    				string_format( "%.3f s", ( double )tresult / 1e9 );

    result->Set( String::NewFromUtf8( isolate, "timing" ), String::NewFromUtf8( isolate, time.c_str() ) );
    result->Set( String::NewFromUtf8( isolate, "todos" ), todos );

    args.GetReturnValue().Set( result );
}

void init( Local<Object> exports ) {
	NODE_SET_METHOD( exports, "searchFile", SearchFile );
}

NODE_MODULE( NODE_GYP_MODULE_NAME, init );