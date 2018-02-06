#!/usr/bin/env node --no-warnings
/** ****************************************************************************************************
 * File: cli.js
 * Project: whatodo
 * @author Nick Soggin <iSkore@users.noreply.github.com> on 05-Feb-2018
 *******************************************************************************************************/
'use strict';

const
    { resolve } = require( 'path' ),
    { version } = require( './package.json' ),
    Whatodo     = require( './index' );

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
    console.log( '    todo [directory to search] [...options]' );
    console.log( '  ' );
    console.log( '  Options:' );
    console.log( '  ' );
    console.log( '    -h, --help       show this help menu' );
    console.log( '    -v, --version    show package version' );
    console.log( '    -d, --dir        directory to check TODOs' );
    console.log( '  ' );
}

function reportError( e ) {
    console.log( '  ' );
    console.log( '  Whatodo: Error Report' );
    console.log( '  ' );
    console.log( `    ${e.split( '\n' ).join( '\n    ' )}` );
    console.log( '  ' );
}

( function( args ) {
    if( args.includes( '-v' ) || args.includes( '--version' ) ) {
        return _version();
    } else if( args.includes( '-h' ) || args.includes( '--help' ) ) {
        return _help();
    }
    
    let
        dir          = resolve( './' ),
        outputFile   = resolve( './TODOS.json' ),
        outputFormat = Whatodo.JSON;
    
    if( args[ 2 ] ) {
        dir = resolve( args[ 2 ] );
    }
    
    if( args.includes( '-o' ) || args.includes( '--output' ) ) {
        const
            i = args.indexOf( '-o' ) === -1 ?
                args.indexOf( '--output' ) + 1 :
                args.indexOf( '-o' ) + 1;
        
        outputFile = resolve( args[ i ] );
    }
    
    if( args.includes( '-f' ) || args.includes( '--format' ) ) {
        const
            i = args.indexOf( '-f' ) === -1 ?
                args.indexOf( '--format' ) + 1 :
                args.indexOf( '-f' ) + 1,
            f = args[ i ];
        
        if( !Object.values( Whatodo ).includes( f ) ) {
            return reportError(
                `${f} is not a supported output format\n` +
                `Supported Formats: ${Object.values( Whatodo ).join( ', ' )}`
            );
        }
        
        outputFormat = args[ i ];
    }
    
    const
        opts = { dir, outputFile, outputFormat };
    
    console.log( `collecting TODOs from ${opts.dir}` );
    console.log( `saving collection to ${opts.outputFile}` );
    
    new Whatodo( opts )
        .initialize()
        .then( inst => inst.run() )
        .then( inst => inst.save() )
        .catch( console.error );
} )( process.argv );
