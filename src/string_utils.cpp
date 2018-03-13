using std::string;
using v8::Isolate;
using v8::Local;
using v8::String;


string v8StringToStd( Local<String> ref ) {
	String::Utf8Value arg( ref );
	return string( *arg );
}

Local<String> stdStringToV8( Isolate *isolate, string ref ) {
	return String::NewFromUtf8( isolate, ref.c_str() );
}