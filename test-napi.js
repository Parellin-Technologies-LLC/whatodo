/** ****************************************************************************************************
 * File: test-napi.js
 * Project: whatodo
 * @author Nick Soggin <iSkore@users.noreply.github.com> on 19-Apr-2018
 *******************************************************************************************************/
'use strict';

const todo = require( 'bindings' )( 'whatodo' );

console.log( '---\n\n' );

console.log( 'todo', todo );
// todo.searchFile( console.log );

todo
	.createPromise()
	.then( n => {
		const end = process.hrtime( start );
		console.log( end );
	} );

todo.concludeCurrentPromise( 'hello world', true );

console.log( '\n\n---' );