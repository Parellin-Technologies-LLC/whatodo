#include "todo.h"

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

Local<Value> SearchLine( Isolate *isolate, string &line, int &i ) {
    const std::regex rx( v8StringToStd( _TODO_PATTERN ) );
    bool containsTodo;

    std::sregex_iterator ri = std::sregex_iterator( line.begin(), line.end(), rx );
    Local<Object> match = Object::New( isolate );

    for( ; ri != std::sregex_iterator(); ++ri ) {
        const std::smatch m = *ri;

        containsTodo = !m.empty();

        if( containsTodo ) {
            const string ms = m.str();

            const int
                llen = line.length(),
                len  = ms.length(),
                pos  = m.position(),
                cpos = pos + len;

            Local<String> comment = stdStringToV8( isolate, line.substr( cpos, llen - cpos ) );

            if( comment == _EMPTY_STRING ) {
            	containsTodo = false;
                break;
            }

            match->Set( _PRIORITY,
				ms.find( ":::" ) != std::string::npos ? _PRIORITY_HIGH :
					ms.find( "::" ) != std::string::npos ? _PRIORITY_MID :
						ms.find( ":" ) != std::string::npos ? _PRIORITY_LOW :
							_PRIORITY_UNKNOWN
			);

			match->Set( _LINE, Number::New( isolate, i ) );
			match->Set( _POSITION, Number::New( isolate, pos ) );
			match->Set( _COMMENT, comment );
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

    if( args.Length() < 1 || !args[ 0 ]->IsString() ) {
    	isolate->ThrowException( Exception::TypeError( String::NewFromUtf8( isolate, "Argument Error - expected string for [filename]" ) ) );
    	return;
    }

    _TODO_PATTERN     = stdStringToV8( isolate, "\\/\\/ ?TODO:?:?:? ?" );
    _PRIORITY         = stdStringToV8( isolate, "priority" );
    _PRIORITY_HIGH    = stdStringToV8( isolate, "HIGH" );
    _PRIORITY_MID     = stdStringToV8( isolate, "MID" );
    _PRIORITY_LOW     = stdStringToV8( isolate, "LOW" );
    _PRIORITY_UNKNOWN = stdStringToV8( isolate, "UNKNOWN" );
    _LINE             = stdStringToV8( isolate, "line" );
    _POSITION         = stdStringToV8( isolate, "position" );
    _COMMENT          = stdStringToV8( isolate, "comment" );
    _EMPTY_STRING     = stdStringToV8( isolate, "" );

    std::string fname = v8StringToStd( args[ 0 ]->ToString() );

    Local<Context> context = isolate->GetCurrentContext();
    Local<Object> obj      = args[ 1 ]->ToObject( context ).ToLocalChecked();
    Local<Array> props     = obj->GetOwnPropertyNames( context ).ToLocalChecked();

    for( int i = 0, l = props->Length(); i < l; i++ ) {
    	Local<Value> localKey = props->Get( i );
    	std::string key = *String::Utf8Value( localKey );

    	if( key == "todoPattern" ) {
    		_TODO_PATTERN = obj->Get( context, localKey ).ToLocalChecked()->ToString();
    	}
	}

    Local<Object> result = Object::New( isolate );
    Local<Array> todos   = Array::New( isolate );

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

            if( !to->IsNull() ) {
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

    result->Set( stdStringToV8( isolate, "file" ), stdStringToV8( isolate, fname ) );
    result->Set( stdStringToV8( isolate, "timing" ), stdStringToV8( isolate, time ) );
    result->Set( stdStringToV8( isolate, "todos" ), todos );

    args.GetReturnValue().Set( result );
}

void RemoveTodo( const FunctionCallbackInfo<Value> &args ) {
    Isolate *isolate = args.GetIsolate();

    if( !args[ 0 ]->IsString() ) {
    	isolate->ThrowException( Exception::TypeError( String::NewFromUtf8( isolate, "Argument Error - expected string [filename]" ) ) );
    	return;
    } else if( !args[ 1 ]->IsNumber() ) {
    	isolate->ThrowException( Exception::TypeError( String::NewFromUtf8( isolate, "Argument Error - expected number [line]" ) ) );
    	return;
    }

    // TODO:: add removeTodo functionality

    args.GetReturnValue().Set( result );
}

void init( Local<Object> exports ) {
	NODE_SET_METHOD( exports, "searchFile", SearchFile );
	NODE_SET_METHOD( exports, "removeTodo", RemoveTodo );
}

NODE_MODULE( NODE_GYP_MODULE_NAME, init );