#include <napi.h>
#include <iostream>
#include <fstream>
#include <regex>
#include <string>
using namespace std;


// TODO: write TODOs with priority levels to html(?) or txt file
// write file
// ofstream myfile;
// myfile.open ("example.txt");
// myfile << "Writing this to a file.\n";
// myfile.close();a

// TODO: allow regex override
string SearchLine( std::string &line ) {
    const std::regex rx( "\\/\\/ ?TODO:?:?:?" );
    const int llen = line.length();

    std::sregex_iterator i = std::sregex_iterator( line.begin(), line.end(), rx );
    string todo_match;

    for( ; i != std::sregex_iterator(); ++i ) {
        const std::smatch m = *i;
        const int pos = m.position();
        todo_match = todo_match + line.substr( pos, llen - pos ) + '\n';
    }

    return todo_match;
}

Napi::Value SearchFile( const Napi::CallbackInfo &args ) {
    Napi::Env env = args.Env();

    // TODO: if file is specified, if folder is specified, else ./
    if( !args[ 0 ].IsString() ) {
        Napi::TypeError::New( env, "Argument Error - expected string" ).ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string fname = args[ 0 ].As<Napi::String>();
    string todos;
    string line;
    ifstream file( fname );

    if ( file.is_open() ) {
        while( getline( file, line ) )
        {
            todos = todos + SearchLine( line );
        }

        file.close();
    } else {
        Napi::TypeError::New( env, "Argument Error - unable to open file" ).ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::String str = Napi::String::New( env, todos );

    return str;
}

Napi::Object Init( Napi::Env env, Napi::Object exports ) {
    exports.Set(
        Napi::String::New( env, "searchFile" ),
        Napi::Function::New( env, SearchFile )
    );

    return exports;
}

NODE_API_MODULE( todo, Init );
