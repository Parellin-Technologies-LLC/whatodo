/** ****************************************************************************************************
 * File: index.test.js
 * Project: whatodo
 * @author Nick Soggin <iSkore@users.noreply.github.com> on 02-Feb-2018
 *******************************************************************************************************/
'use strict';

const
    chai           = require( 'chai' ),
    chaiAsPromised = require( 'chai-as-promised' ),
    expect         = chai.expect,
    { join }       = require( 'path' ),
    fs             = require( 'fs' );
-
    
    chai.use( chaiAsPromised );

describe( 'whatodo - tests', () => {
    const
        Whatodo  = require( '../index' ),
        testFile = join( process.cwd(), 'test', 'test.cc' );
    
    let todos = new Whatodo( { dir: './test/' } );
    
    it( 'should have config fields', () => {
        expect( todos ).to.have.property( 'dir' );
        expect( todos ).to.have.property( 'ignore' );
        expect( todos ).to.have.property( 'ignoreRx' );
        expect( todos ).to.have.property( 'todoFormat' );
    } );
    
    it( 'should initialize and read files',
        () => expect(
            todos.initialize().then( inst => todos = inst )
        )
            .to.eventually.have.property( 'files' )
            .and.to.be.an( 'array' )
    );
    
    it( 'searchFile should capture TODO comments',
        () => {
            expect( todos.searchFile( testFile ) )
                .to.be.an( 'array' )
                .and.deep.eq( [
                    { priority: 'LOW', line: 1, position: 0, comment: 'low priority' },
                    { priority: 'MID', line: 2, position: 0, comment: 'mid priority' },
                    { priority: 'HIGH', line: 3, position: 0, comment: 'high priority' }
                ] );
        }
    );
    
    it( 'should search the listed files and collect TODOs',
        () => {
            expect( todos.run().then( inst => todos = inst ) )
                .to.eventually.have.property( 'todos' )
                .and.to.be.an( 'array' );
        }
    );
    
    it( 'should capture file "test.cc"',
        () => {
            const todo = todos.todos[ 0 ];
            
            expect( todo )
                .to.have.property( 'fname' )
                .and.eq( join( todos.dir, 'test.cc' ) );
            
            expect( todo )
                .to.have.property( 'isDirectory' )
                .and.eq( false );
        }
    );
    
    it( 'should capture file size of "test.cc"',
        () => {
            const todo = todos.todos[ 0 ];
            
            expect( todos.fstats( todo.fname ) )
                .to.eventually.have.property( 'size' )
                .and.eq( todo.size );
        }
    );
    
    it( 'should capture timing and be less than 150 microseconds (μs)',
        () => {
            const todo = todos.todos[ 0 ];
            
            expect( +( todo.timing.match( /\d+.\d+/ )[ 0 ] ) )
                .to.be.lt( 150 );
        }
    );
    
    it( 'should save output to the specified output file',
        () => {
            expect( todos.save() ).to.eventually.eq( `${todos.outputFile} SAVED` );
        }
    );
    
    it( `should create file named ${todos.outputFile}`,
        () => {
            const outputExists = fs.existsSync( todos.outputFile );
            expect( outputExists ).to.eq( true );
        }
    );
    
    it( `should create file with correct parameters`,
        () => {
            let outputData = fs.readFileSync( todos.outputFile );
            outputData     = outputData.toString( 'utf8' );
            outputData     = JSON.parse( outputData );
            
            expect( outputData ).to.deep.eq( todos.todos );
        }
    );
} );
