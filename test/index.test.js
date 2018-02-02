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
    
    todos
        .initialize()
        .then( inst => inst.run() )
        .then( inst => inst.save( './TODOS.json' ) )
        .then( console.log )
        .catch( console.error );
} );
