/** ****************************************************************************************************
 * File: index.test.js
 * Project: whatodo
 * @author Nick Soggin <iSkore@users.noreply.github.com> on 02-Feb-2018
 *******************************************************************************************************/
'use strict';

const
    { join }       = require( 'path' ),
    chai           = require( 'chai' ),
    chaiAsPromised = require( 'chai-as-promised' ),
    expect         = chai.expect;

chai.use( chaiAsPromised );

describe( 'whatodo', () => {
    const Whatodo = require( '../index' );
    const todos = new Whatodo();
    
    const start = process.hrtime();
    
    todos
        .initialize()
        .then( inst => inst.run() )
        .then( d => {
            const end = process.hrtime( start );
            console.log( JSON.stringify( d, null, 4 ) );
            console.log( `Executed in: ${( end[ 0 ] + end[ 1 ] ) / 1e3} μs` );
        } )
        .catch( console.error );
} );
