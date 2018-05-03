/** ****************************************************************************************************
 * File: test-napi.js
 * Project: whatodo
 * @author Nick Soggin <iSkore@users.noreply.github.com> on 19-Apr-2018
 *******************************************************************************************************/
'use strict';

const
	{ resolve } = require( 'path' ),
	todo        = require( 'bindings' )( 'whatodo' );

console.log( '---\n\n' );

console.log( 'todo', todo );
todo.initialize().then( console.log ).then( () => console.log( 'www' ) );
console.log( 'hhh' );

const fpath = resolve( './test/test.cc' );
todo
	.searchFile( fpath, {
		todoPattern: '\\/\\/ ?TODO:?:?:? ?'
	} )
	.then( d => ( console.log( 'TODOBJ', d ), d ) )
	.then( d => ( console.log( 'stuff', d.filename ), d ) )
	.then( d => ( console.log( 'stuff', d.todoPattern ), d ) )
	.then( d => console.log( require( 'util' ).inspect( d, { showHidden: true, depth: 0 } ) ) )
	.catch( console.error );

console.log( 'ggg' );


// todo.searchFile( console.log );
// todo
// 	.createPromise()
// 	.then( n => console.log( 'log', n ) )
// 	.catch( e => console.error( 'error', e ) );
//
// // todo.concludeCurrentPromise( 'hello world', true );
// todo.concludeCurrentPromise( 'hello world', true );
//
// todo.searchFile( './index.jss' ).then( console.log );
// console.log( todo.searchFile( './index.jss' ) );

console.log( '\n\n---' );
