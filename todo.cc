#include <napi.h>
#include <iostream>
#include <fstream>
#include <regex>
#include <string>

using namespace std;

enum PRIORITY {
    UNKNOWN = 0,
    HIGH    = 1,
    MID     = 2,
    LOW     = 3
};

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
        PRIORITY priority;

        std::size_t found = ms.find( ":::" );
        if( found != std::string::npos )
            priority = HIGH;

        match = line.substr( pos, llen - pos );
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
