/** ****************************************************************************************************
 * File: index.js
 * Project: what-todo
 * @author Nick Soggin <iSkore@users.noreply.github.com> on 01-Feb-2018
 *******************************************************************************************************/
'use strict';

const
    { join } = require( 'path' ),
    todo     = require( 'bindings' )( 'todo' ),
    start    = process.hrtime(),
    todos    = todo.searchFile( join( process.cwd(), './README.md' ) ),
    end      = process.hrtime( start );


/**
 * searchFile
 * currently, only exporting `searchFile` - in progress
 */
module.exports = todo.searchFile;

console.log( `${todos}\nExecuted in: ${( end[ 0 ] + end[ 1 ] ) / 1e3} μs` );
