/** ****************************************************************************************************
 * File: utils.js
 * Project: whatodo
 * @author Nick Soggin <iSkore@users.noreply.github.com> on 08-Mar-2018
 *******************************************************************************************************/
'use strict';

function convertHighResolutionTime( hrtime ) {
	if( !Array.isArray( hrtime ) || hrtime.length !== 2 ) {
		throw new Error( 'Argument Error - expected hrtime_tuple' );
	}
	
	return ( hrtime[ 0 ] * 1e9 ) + hrtime[ 1 ];
}

module.exports = {
	convertHighResolutionTime
};
