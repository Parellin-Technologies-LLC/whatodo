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
	Whatodo      = require( './index' );

function _version() {
	console.log( `v${version}` );
}

function _help() {
	console.log( '  ' );
	console.log( `  Whatodo v${version}` );
	console.log( '  ' );
	console.log( '  Whatodo is a NPM Module designed to quickly and efficiently extract your' );
	console.log( '  TODO comments from your code and organize them in a simple & readable format' );
	console.log( '  ' );
	console.log( '  Usage:' );
	console.log( '  ' );
	console.log( '    todo [input file/folder] [...options]' );
	console.log( '  ' );
	console.log( '  Options:' );
	console.log( '  ' );
	console.log( '    -h, --help      show this help menu' );
	console.log( '    -v, --version   show package version' );
	console.log( '    -i, --input     directory or file to check TODOs  (default: ./)' );
	console.log( '    -o, --output    file to save TODOs                (default: stdout)' );
	console.log( '    -f, --format    format to save TODOs              (default: STDOUT, "JSON")' );
	console.log( '    -p, --pattern   pattern to capture TODOs          (default: "\\/\\/ ?TODO:?:?:? ?")' );
	console.log( '    -m, --maximum   maximum file size allowed         (default: "1 MB", 512KiB)' );
	console.log( '  ' );
}

function reportError( e ) {
	console.log( '  ' );
	console.log( '  Whatodo: Error Report' );
	console.log( '  ' );
	console.log( `    ${( e.stackTrace || e.message || e ).split( '\n' ).join( '\n    ' )}` );
	console.log( '  ' );
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
						reportError( `Argument Error - must specify input for ${item}` );
					} else {
						r.input = resolve( input );
					}
				} else if( item === '-o' || item === '--output' ) {
					const output = arr[ i + 1 ];
					
					if( !output ) {
						reportError( `Argument Error - must specify output location for ${item}` );
					} else {
						r.outputFile = resolve( output );
					}
				} else if( item === '-f' || item === '--format' ) {
					const format = arr[ i + 1 ];
					
					if( !format ) {
						reportError(
							`Argument Error - must specify format type for ${item} [ ${Object.values( Whatodo.FORMAT )
								.join( ', ' )} ]`
						);
					} else if( !Object.values( Whatodo.FORMAT ).includes( format ) ) {
						reportError(
							`${format} is not a supported output format\n` +
							`Supported Formats: [ ${Object.values( Whatodo.FORMAT ).join( ', ' )} ]`
						);
					} else {
						r.outputFormat = format;
					}
				} else if( item === '-p' || item === '--pattern' ) {
					const pattern = arr[ i + 1 ];
					
					if( !pattern ) {
						reportError( `Argument Error - must specify pattern for ${item} [ "\\/\\/ ?TODO:?:?:? ?" ]` );
					} else {
						r.todoPattern = pattern;
					}
				} else if( item === '-m' || item === '--maximum' ) {
					const maximum = arr[ i + 1 ];
					
					if( !maximum ) {
						reportError( `Argument Error - must specify maximum value for ${item} [ 1MB ]` );
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
