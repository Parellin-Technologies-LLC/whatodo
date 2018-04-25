using std::string;
using Napi::Isolate;
using Napi::String;

string v8StringToStd( Napi::String ref ) {
	Napi::String arg(env,  ref );
	return string( *arg );
}

Napi::String stdStringToV8( Isolate *isolate, string ref ) {
	return String::NewFromUtf8( isolate, ref.c_str() );
}

string replace( string line, const string& substr, const string& replace_with )
{
	string::size_type pos = 0 ;
	while( ( pos = line.find( substr, pos ) ) != string::npos )
	{
		line.replace( pos, substr.size(), replace_with ) ;
		pos += replace_with.size() ;
	}

	return line;
}
