using std::string;
using std::vector;

string string_format( const string fmt, ... ) {
    vector< char > str( 100, '\0' );
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