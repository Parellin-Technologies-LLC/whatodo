/** ****************************************************************************************************
 * File: binding.test.js
 * Project: whatodo
 * @author Nick Soggin <iSkore@users.noreply.github.com> on 25-Apr-2018
 *******************************************************************************************************/
'use strict';

const WhatodoNapi = require( '../../binding.js' );
const assert      = require( 'assert' );

assert( WhatodoNapi, 'The expected module is undefined' );

function testBasic()
{
	const instance = new WhatodoNapi( 'mr-yeoman' );
	assert( instance.greet, 'The expected method is not defined' );
	assert.strictEqual( instance.greet( 'kermit' ), 'mr-yeoman', 'Unexpected value returned' );
}

function testInvalidParams()
{
	const instance = new WhatodoNapi();
}

assert.doesNotThrow( testBasic, undefined, 'testBasic threw an expection' );
assert.throws( testInvalidParams, undefined, 'testInvalidParams didn\'t throw' );

console.log( 'Tests passed- everything looks OK!' );
