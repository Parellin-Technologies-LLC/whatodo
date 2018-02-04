/** ****************************************************************************************************
 * File: index.test.js
 * Project: whatodo
 * @author Nick Soggin <iSkore@users.noreply.github.com> on 02-Feb-2018
 *******************************************************************************************************/
'use strict';

const
    chai           = require( 'chai' ),
    chaiAsPromised = require( 'chai-as-promised' ),
    expect         = chai.expect;

chai.use( chaiAsPromised );

describe( 'whatodo', () => {
    const Whatodo = require( '../index' );
    const todos   = new Whatodo();

    it( 'should have config fields', () => {
        expect( todos ).to.have.property( 'dir' );
        expect( todos ).to.have.property( 'ignore' );
        expect( todos ).to.have.property( 'ignoreRx' );
        expect( todos ).to.have.property( 'todoFormat' );
    } );

    it( 'should initialize and read files',
        () => expect( todos.initialize() ).to.eventually.have.property( 'files' )
    );

    // .then( inst => inst.run() )
    // .then( inst => inst.save( './TODOS.json' ) )
    // .then( console.log )
    // .catch( console.error );
} );
