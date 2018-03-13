/** ****************************************************************************************************
 * File: utils.js
 * Project: whatodo
 * @author Nick Soggin <iSkore@users.noreply.github.com> on 08-Mar-2018
 *******************************************************************************************************/
'use strict';

/**
 * @typedef {number[]} hrtime
 * @description
 *     A "tuple" assumed to be a result of calling for high resolution time <code>process.hrtime</code><br/>
 *     typically, a resulting <code>hrtime</code> should be the returned value of
 *     <code>process.hrtime( process.hrtime() );</code> where the inner <code>hrtime</code>
 *     is likely called before a function is run to calculate the time it took to run a process.
 *     tuple is assumed to be in nanoseconds
 * @example
 * [ 22727, 841838045 ]
 */

/**
 * @typedef {number} bytes
 * @description
 *     A number expected to be evaluated as bytes
 * @example
 * 1024
 */

/**
 * convertHighResolutionTime
 * @description
 * converts a "final" high resolution time stamp into nanoseconds
 * @param {hrtime} hrtime - high resolution time tuple returned from <code>process.hrtime</code>
 * @returns {number}
 * nanoseconds based on passed in high resolution time tuple
 * @example
 * const
 *     start = process.hrtime(),
 *     end   = process.hrtime( start );
 *
 * convertHighResolutionTime( end ); // -> 2000
 */
function convertHighResolutionTime( hrtime ) {
	if( !Array.isArray( hrtime ) || hrtime.length !== 2 ) {
		throw new Error( 'Argument Error - expected hrtime_tuple' );
	}
	
	return ( hrtime[ 0 ] * 1e9 ) + hrtime[ 1 ];
}

/**
 * bytesToSize
 * @description
 * Convert bytes to human readable format
 * @param {bytes} bytes - unit in bytes to parse
 * @returns {string} - pretty string in the format [n unit]
 * @example
 * bytesToSize( 1073741824 ) // -> 1 GB
 */
function bytesToSize( bytes ) {
	if( !bytes ) {
		return '0 Byte';
	}
	
	const
		sizes = [ 'Bytes', 'KB', 'MB', 'GB', 'TB' ],
		i     = ~~( Math.log( bytes ) / Math.log( 1024 ) );
	
	return Math.round( bytes / Math.pow( 1024, i ) ) + ' ' + sizes[ i ];
}

/**
 * sizeToBytes
 * @description
 * Convert human readable format to bytes
 * <h5>Recognized conversion units:</h5>
 * <table class="params">
 *     <thead><tr><th>(Unit)</th><th>[Standard Base 10]</th><th>[Digital Storage Unit 2ⁿ]</th></tr></thead>
 *     <tbody>
 *         <tr><td>(B)</td><td>Bytes</td><td>Bytes</td></tr>
 *         <tr><td>(kB|KiB)</td><td>Kilobytes</td><td>Kibibytes</td></tr>
 *         <tr><td>(mB|MiB)</td><td>Megabytes</td><td>Megibytes</td></tr>
 *         <tr><td>(gB|GiB)</td><td>Gigabytes</td><td>Gigibytes</td></tr>
 *         <tr><td>(tB|TiB)</td><td>Terabytes</td><td>Teribytes</td></tr>
 *     </tbody>
 * </table>
 * @param {string|number} bytes - string represending the bytes and unit to calculate the conversion
 * @returns {bytes} - returns the number of bytes represented by the string
 * @example
 * sizeToBytes( '1 KB' )        // -> 1000
 * sizeToBytes( '1 KiB' )       // -> 1024
 * sizeToBytes( '1 Kilobytes' ) // -> 1000
 * sizeToBytes( '1 Kibibytes' ) // -> 1024
 */
function sizeToBytes( bytes ) {
	let match;
	
	return !bytes ? 0 : bytes === +bytes ? bytes :
		( match = /^(\d+) ?(tB|TiB|Ter[a,i]bytes?)$/gim.exec( bytes ) ) ?
			/tB|tera/gi.test( match[ 2 ] ) ? match[ 1 ] * 1000000000000 : match[ 1 ] * 1099511627776 :
			( match = /^(\d+) ?(gB|GiB|Gig[a,i]bytes?)$/gim.exec( bytes ) ) ?
				/gB|giga/gi.test( match[ 2 ] ) ? match[ 1 ] * 1000000000 : match[ 1 ] * 1073741824 :
				( match = /^(\d+) ?(mB|MiB|Meg[a,i]bytes?)$/gim.exec( bytes ) ) ?
					/mB|mega/gi.test( match[ 2 ] ) ? match[ 1 ] * 1000000 : match[ 1 ] * 1048576 :
					( match = /^(\d+) ?(kB|KiB|Ki(?:lo|bi)bytes?)$/gim.exec( bytes ) ) ?
						/kB|kilo/gi.test( match[ 2 ] ) ? match[ 1 ] * 1000 : match[ 1 ] * 1024 :
						( match = /^(\d+) ?(?:B|Bytes?)$/gim.exec( bytes ) ) ? +match[ 1 ] : 0;
}

function isOption( arg ) {
	return /-+\w+/.test( arg );
}

module.exports = {
	convertHighResolutionTime,
	bytesToSize,
	sizeToBytes,
	isOption
};
