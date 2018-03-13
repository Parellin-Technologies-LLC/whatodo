using std::string;
using v8::Isolate;
using v8::Local;
using v8::String;



Local<Value> SearchLine( Isolate *isolate, string &line, int &i ) {
    const regex rx( v8StringToStd( _TODO_PATTERN ) );
    bool containsTodo;

    sregex_iterator ri = sregex_iterator( line.begin(), line.end(), rx );
    Local<Object> match = Object::New( isolate );

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

            Local<String> comment = stdStringToV8( isolate, line.substr( cpos, llen - cpos ) );

            if( comment == _EMPTY_STRING ) {
            	containsTodo = false;
                break;
            }

            match->Set( _PRIORITY,
				ms.find( ":::" ) != string::npos ? _PRIORITY_HIGH :
					ms.find( "::" ) != string::npos ? _PRIORITY_MID :
						ms.find( ":" ) != string::npos ? _PRIORITY_LOW :
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