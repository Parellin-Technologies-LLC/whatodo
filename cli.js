#!/usr/bin/env node --no-warnings
/** ****************************************************************************************************
 * File: cli.js
 * Project: whatodo
 * @author Nick Soggin <iSkore@users.noreply.github.com> on 05-Feb-2018
 *******************************************************************************************************/
'use strict';

const
	{ resolve }  = require( 'path' ),
	{ version }  = require( './package.json' ),
	{ isOption } = require( './utils' ),
	Whatodo      = require( './index' ),
	tab          = '  ',
	endl         = '\n',
	dtab         = tab + tab,
	tendl        = tab + endl;

function _version() {
	console.log( `v${ version }${ endl }` );
}

function _help() {
	let help = '';
	
	help += tendl;
	help += `${ tab }Whatodo v${ version }${ endl }`;
	help += tendl;
	help += `${ tab }Whatodo is a NPM Module designed to quickly and efficiently extract your${ endl }`;
	help += `${ tab }TODO comments from your code and organize them in a simple & readable format${ endl }`;
	help += tendl;
	help += `${ tab }Usage:${ endl }`;
	help += tendl;
	help += `${ dtab }todo [input file/folder] [...options]${ endl }`;
	help += tendl;
	help += `${ tab }Options:${ endl }`;
	help += tendl;
	help += `${ dtab }-h, --help      show this help menu${ endl }`;
	help += `${ dtab }-v, --version   show package version${ endl }`;
	help += `${ dtab }-i, --input     directory or file to check TODOs  (default: ./)${ endl }`;
	help += `${ dtab }-o, --output    file to save TODOs                (default: STDOUT)${ endl }`;
	help += `${ dtab }-f, --format    format to save/print TODOs        (default: STDOUT, JSON)${ endl }`;
	help += `${ dtab }-p, --pattern   pattern to capture TODOs          (default: "\\/\\/ ?TODO:?:?:? ?")${ endl }`;
	help += `${ dtab }-m, --maximum   maximum file size allowed         (default: "1 MB", 512KiB)${ endl }`;
	
	console.log( help );
}

function reportError( e ) {
	let report = '';
	
	report += tendl;
	report += `${ tab }Whatodo: Error Report${ endl }`;
	report += tendl;
	report += `${ dtab }${ ( e.stackTrace || e.stack || e ).split( '\\n' ).join( '\\n    ' ) }${ endl }`;
	
	console.log( report );
	process.exit( 1 );
}

( function( args ) {
	if( args.includes( '-v' ) || args.includes( '--version' ) ) {
		return _version();
	} else if( args.includes( '-h' ) || args.includes( '--help' ) ) {
		return _help();
	}
	
	const
		opts = args.reduce(
			( r, item, i, arr ) => {
				if( item === '-i' || item.includes( '--input' ) ) {
					const input = arr[ i + 1 ];
					
					if( !input ) {
						reportError( `Argument Error - must specify input for ${ item }` );
					} else {
						r.input = resolve( input );
					}
				} else if( item === '-o' || item === '--output' ) {
					const output = arr[ i + 1 ];
					
					if( !output ) {
						reportError( `Argument Error - must specify output location for ${ item }` );
					} else {
						r.outputFile = resolve( output );
					}
				} else if( item === '-f' || item === '--format' ) {
					const format = arr[ i + 1 ];
					
					if( !format ) {
						const availableFormats = Object.values( Whatodo.FORMAT ).join( ', ' );
						reportError(
							`Argument Error - must specify format type for ${ item } [ ${ availableFormats } ]`
						);
					} else if( !Object.values( Whatodo.FORMAT ).includes( format ) ) {
						reportError(
							`${ format } is not a supported output format\n` +
							`Supported Formats: [ ${ Object.values( Whatodo.FORMAT ).join( ', ' ) } ]`
						);
					} else {
						r.outputFormat = format;
					}
				} else if( item === '-p' || item === '--pattern' ) {
					const pattern = arr[ i + 1 ];
					
					if( !pattern ) {
						reportError( `Argument Error - must specify pattern for ${ item } [ "\\/\\/ ?TODO:?:?:? ?" ]` );
					} else {
						r.todoPattern = pattern;
					}
				} else if( item === '-m' || item === '--maximum' ) {
					const maximum = arr[ i + 1 ];
					
					if( !maximum ) {
						reportError( `Argument Error - must specify maximum value for ${ item } [ 1MB ]` );
					} else {
						r.maximumFileSize = maximum;
					}
				}
				
				return r;
			}, {
				input: resolve( isOption( args[ 2 ] ) ? args[ 2 ] : './' ),
				todoPattern: '\\/\\/ ?TODO:?:?:? ?',
				outputFormat: Whatodo.FORMAT.STDOUT
			}
		);
	
	return new Whatodo( opts )
		.initialize()
		.then( inst => inst.run() )
		.then( inst => {
			if( opts.outputFile ) {
				return inst.save();
			} else {
				return inst.print();
			}
		} )
		.catch( reportError );
} )( process.argv );
