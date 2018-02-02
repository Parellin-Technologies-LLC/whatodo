#include <napi.h>
#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <chrono>

using namespace std;

// TODO: allow regex override
Napi::Value SearchLine( Napi::Env &env, string &line, int &i ) {
    const std::regex rx( "\\/\\/ ?TODO:?:?:? ?" );
    const int llen = line.length();

    std::sregex_iterator ri = std::sregex_iterator( line.begin(), line.end(), rx );

    Napi::Object match;
    bool matchFound;

    for( ; ri != std::sregex_iterator(); ++ri ) {
        const std::smatch m = *ri;
        const string ms     = m.str();
        const int len       = ms.length();
        const int pos       = m.position() + len;

        matchFound = m.empty();

        if( !matchFound ) {
            match = Napi::Object::New( env );

            std::size_t found = ms.find( ":::" );
            if( found != std::string::npos ) {
                match.Set( "priority", "HIGH" );
            } else {
                std::size_t found = ms.find( "::" );
                if( found != std::string::npos ) {
                    match.Set( "priority", "MID" );
                } else {
                    std::size_t found = ms.find( ":" );
                    if( found != std::string::npos ) {
                        match.Set( "priority", "LOW" );
                    } else {
                        match.Set( "priority", "UNKNOWN" );
                    }
                }
            }

            match.Set( "line", i );
            match.Set( "position", m.position() );
            match.Set( "comment", line.substr( pos, llen - pos ) );
        }
    }

    if( !matchFound ) {
        return match;
    } else {
        return Napi::Value();
    }
}

Napi::Value SearchFile( const Napi::CallbackInfo &args ) {
    Napi::Env env = args.Env();
    std::string fname = args[ 0 ].As<Napi::String>();

    if( !args[ 0 ].IsString() ) {
        Napi::TypeError::New( env, "Argument Error - expected string" ).ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::Array todos = Napi::Array::New( env );

    auto begin = std::chrono::high_resolution_clock::now();

    int i = 0;
    int n = 0;
    string line;
    ifstream file( fname );

    if( file.is_open() ) {
        while( getline( file, line ) )
        {
            ++i;
            Napi::Value to = SearchLine( env, line, i );

            if( !to.IsEmpty() ) {
                todos[ n++ ] = to;
            }
        }

        file.close();
    } else {
        Napi::TypeError::New( env, "Argument Error - unable to open file" ).ThrowAsJavaScriptException();
        return env.Null();
    }

    auto end = std::chrono::high_resolution_clock::now();

    todos[ "time" ] = std::chrono::duration_cast<std::chrono::nanoseconds>( end - begin ).count();

    return todos;
}

Napi::Object Init( Napi::Env env, Napi::Object exports ) {
    exports.Set(
        Napi::String::New( env, "searchFile" ),
        Napi::Function::New( env, SearchFile )
    );

    return exports;
}

NODE_API_MODULE( todo, Init );
