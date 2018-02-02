#include <napi.h>
#include <iostream>
#include <fstream>
#include <regex>
#include <string>

#include <chrono>

using namespace std;

// TODO: allow regex override
string SearchLine( string &line ) {
    const std::regex rx( "\\/\\/ ?TODO:?:?:? ?" );
    const int llen = line.length();

    std::sregex_iterator i = std::sregex_iterator( line.begin(), line.end(), rx );
    string match;

    for( ; i != std::sregex_iterator(); ++i ) {
        const std::smatch m = *i;
        const string ms     = m.str();
        const int pos       = m.position() + ms.length();
        string priority;

        // TODO: move Napi::Object to this method
        std::size_t found = ms.find( ":::" );
        if( found != std::string::npos ) {
            priority = "0";
        } else {
            std::size_t found = ms.find( "::" );
            if( found != std::string::npos ) {
                priority = "1";
            } else {
                std::size_t found = ms.find( ":" );
                if( found != std::string::npos ) {
                    priority = "2";
                } else {
                    priority = "3";
                }
            }
        }

        match = priority + line.substr( pos, llen - pos );
    }

    return match;
}

Napi::Value SearchFile( const Napi::CallbackInfo &args ) {
    Napi::Env env = args.Env();

    // TODO:if file is specified, if folder is specified, else ./
    if( !args[ 0 ].IsString() ) {
        Napi::TypeError::New( env, "Argument Error - expected string" ).ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string fname = args[ 0 ].As<Napi::String>();
    Napi::Array todos = Napi::Array::New( env );

    auto begin = std::chrono::high_resolution_clock::now();

    int n = 0;
    int i = 0;
    string line;
    ifstream file( fname );

    if( file.is_open() ) {
        while( getline( file, line ) )
        {
            ++n;
            string comment = SearchLine( line );

            if( !comment.empty() ) {
                Napi::Object to = Napi::Object::New( env );
                to.Set( "line", n );
                to.Set( "comment", comment );
                todos[ i++ ] = to;
            }
        }

        file.close();
    } else {
        Napi::TypeError::New( env, "Argument Error - unable to open file" ).ThrowAsJavaScriptException();
        return env.Null();
    }

    auto end = std::chrono::high_resolution_clock::now();
    todos[ "time" ] = std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count();

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
