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
	fs             = require( 'fs' ),
	{ join }       = require( 'path' ),
	{ spawn }      = require( 'child_process' ),
	{ version }    = require( '../package.json' );

chai.use( chaiAsPromised );

function spawnCLI( cmd, ...args ) {
	return new Promise(
		( res, rej ) => {
			const
				op   = spawn( cmd, args ),
				body = [],
				err  = [];
			
			op.stdout.on( 'data', data => body.push( data ) );
			op.stderr.on( 'data', data => err.push( data ) );
			op.on( 'error', rej );
			op.on( 'close',
				() => {
					const
						e = Buffer.concat( err ).toString(),
						d = Buffer.concat( body ).toString();
					
					if( e && /Warning: N-API is an experimental/.test( e ) ) {
						res( d );
					} else if( e ) {
						rej( e );
					} else {
						res( d );
					}
				}
			);
		}
	);
}

describe( 'whatodo - tests', () => {
	const
		Whatodo  = require( '../index' ),
		testFile = join( process.cwd(), 'test', 'test.cc' ),
		copyBack = join( process.cwd(), 'test', 'test_copy.cc' );
	
	let todos = new Whatodo( {
		input: testFile,
		outputFile: 'TODOS.json'
	} );
	
	it( 'should have config fields', () => {
		expect( todos ).to.have.property( 'input' );
		expect( todos ).to.have.property( 'ignore' );
		expect( todos ).to.have.property( 'ignoreRx' );
		expect( todos ).to.have.property( 'todoPattern' );
	} );
	
	it( 'should remove todo from file',
		() => {
			const output = todos.removeTodo( testFile, 6 );
			expect( output ).to.eq( true );
			
			expect( fs.readFileSync( testFile ).toString( 'utf8' ) )
				.to.not.include.string( '// TODO::: REMOVE THIS' );
		}
	);
	
	it( 'should initialize and read files',
		() => expect(
			todos.initialize().then( inst => todos = inst )
		)
			.to.eventually.have.property( 'files' )
			.and.to.be.an( 'array' )
	);
	
	it( 'searchFile should capture TODO comments',
		() => {
			const output = todos.searchFile( testFile );
			
			expect( output ).to.be.an( 'object' );
			expect( output ).to.have.property( 'timing' );
			expect( output ).to.have.property( 'file' ).and.eq( testFile );
			expect( output ).to.have.property( 'todos' ).and.deep.eq( [
				{ priority: 'low', line: 1, position: 0, comment: 'todo low priority' },
				{ priority: 'mid', line: 2, position: 0, comment: 'todo mid priority' },
				{ priority: 'high', line: 3, position: 0, comment: 'todo high priority' }
			] );
		}
	);
	
	it( 'searchFile RegEx Pattern override should capture TEST comments',
		() => {
			const output = todos.searchFile( testFile, { todoPattern: '\\/\\/ ?TEST:?:?:? ?' } );
			
			expect( output ).to.be.an( 'object' );
			expect( output ).to.have.property( 'timing' );
			expect( output ).to.have.property( 'file' ).and.eq( testFile );
			expect( output ).to.have.property( 'todos' ).and.deep.eq( [
				{ priority: 'low', line: 4, position: 0, comment: 'test low priority' },
				{ priority: 'mid', line: 5, position: 0, comment: 'test mid priority' },
				{ priority: 'high', line: 6, position: 0, comment: 'test high priority' }
			] );
		}
	);
	
	it( 'should search the listed files and collect TODOs',
		() => expect( todos.run().then( inst => todos = inst ) )
			.to.eventually.have.property( 'todos' )
			.and.to.be.an( 'array' )
	);
	
	it( 'should capture file "test.cc"',
		() => {
			const todo = todos.todos[ 0 ];
			
			expect( todo )
				.to.have.property( 'file' )
				.and.eq( testFile );
		}
	);
	
	it( 'should capture file size of "test.cc"',
		() => {
			const todo = todos.todos[ 0 ];
			
			expect( todos.fstats( todo.file ) )
				.to.eventually.have.property( 'size' )
				.and.eq( todo.size );
		}
	);
	
	it( 'should capture timing and be less than 250 microseconds (μs)',
		() => {
			const todo = todos.todos[ 0 ];
			
			expect( +( todo.timing.match( /\d+.\d+/ )[ 0 ] ) )
				.to.be.lt( 250 );
		}
	);
	
	it( 'should save output to the specified output file',
		() => expect( todos.save() ).to.eventually.eq( todos )
	);
	
	it( `should create file named ${todos.outputFile}`,
		() => {
			const outputExists = fs.existsSync( todos.outputFile );
			expect( outputExists ).to.eq( true );
		}
	);
	
	it( 'should create file with correct parameters',
		() => {
			let outputData = fs.readFileSync( todos.outputFile );
			outputData     = outputData.toString( 'utf8' );
			outputData     = JSON.parse( outputData );
			
			expect( outputData ).to.deep.eq( todos.todos );
		}
	);
	
	it( 'should report help menu',
		() => expect( spawnCLI( 'node', './cli.js', '-h' ) )
			.to.eventually.have
			.string( '-h, --help' )
	);
	
	it( 'should report version',
		() => expect( spawnCLI( 'node', './cli.js', '-v' ) )
			.to.eventually.have
			.string( `v${version}` )
	);
	
	it( 'should report error if incorrect format is used',
		() => expect( spawnCLI( 'node', './cli.js', './', '-f', 'JSONS' ) )
			.to.eventually.have
			.string( 'not a supported output' )
	);
	
	it( 'should report todos in STDOUT format with REGEX "\\/\\/ ?TEST:?:?:? ?"',
		() => expect(
			spawnCLI( 'node', './cli.js', '-i', './test/test.cc', '-f', 'STDOUT', '-p', '\\/\\/ ?TEST:?:?:? ?' )
		)
			.to.eventually.have
			.string( '[low]  line: 4 - test low priority' )
			.and.string( '[mid]  line: 5 - test mid priority' )
			.and.string( '[high] line: 6 - test high priority' )
	);
	
	it( 'should report todos in JSON format',
		() => expect( spawnCLI( 'node', './cli.js', '-i', './test/test.cc', '-f', 'JSON' ) )
			.to.eventually.have
			.and.string( '"comment": "todo low priority"' )
			.and.string( '"comment": "todo mid priority"' )
			.and.string( '"comment": "todo high priority"' )
	);
	
	after(
		() => fs.createReadStream( copyBack ).pipe( fs.createWriteStream( testFile ) )
	);
} );
