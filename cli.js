#!/usr/bin/env node --no-warnings
/** ****************************************************************************************************
 * File: cli.js
 * Project: whatodo
 * @author Nick Soggin <iSkore@users.noreply.github.com> on 05-Feb-2018
 *******************************************************************************************************/
'use strict';

const
    { resolve }    = require( 'path' ),
    { version }    = require( './package.json' ),
    Whatodo        = require( './index' );

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
    console.log( '  Options:' );
    console.log( '  ' );
    console.log( '    -h, --help       show this help menu' );
    console.log( '    -v, --version    show package version' );
    console.log( '    -d, --dir        directory to check TODOs' );
    console.log( '  ' );
}

let
    dir        = resolve( './' ),
    outputFile = resolve( './TODOS.json' );

if( process.argv.includes( '-v' ) || process.argv.includes( '--version' ) ) {
    return _version();
} else if( process.argv.includes( '-h' ) || process.argv.includes( '--help' ) ) {
    return _help();
}

if( process.argv[ 2 ] ) {
    dir = resolve( process.argv[ 2 ] );
}

if( process.argv.includes( '-o' ) || process.argv.includes( '--output' ) ) {
    const
        i = process.argv.indexOf( '-o' ) === -1 ?
            process.argv.indexOf( '--output' ) + 1 :
            process.argv.indexOf( '-o' ) + 1;
    
    outputFile = resolve( process.argv[ i ] );
}

const
    opts = { dir, outputFile };

console.log( `collecting TODOs from ${opts.dir}` );
console.log( `saving collection to ${opts.outputFile}` );

new Whatodo( opts )
    .initialize()
    .then( inst => inst.run() )
    .then( inst => inst.save() )
    .catch( console.error );