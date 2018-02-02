/** ****************************************************************************************************
 * File: index.js
 * Project: whatodo
 * @author Nick Soggin <iSkore@users.noreply.github.com> on 01-Feb-2018
 *******************************************************************************************************/
'use strict';

const
    { readdir, stat } = require( 'fs' ),
    { resolve, join } = require( 'path' ),
    todo              = require( 'bindings' )( 'todo' );

class Whatodo
{
    constructor( opts )
    {
        opts = opts || {};
        
        this.dir        = opts.dir || process.cwd();
        this.ignore     = opts.ignore || [ 'node_modules', '.git', '.idea' ];
        this.ignoreRx   = new RegExp( `^${this.ignore.join( '$|^' )}$` );
        this.todoFormat = opts.todoFormat || '\\/\\/ ?TODO:?:?:?';
    }
    
    initialize()
    {
        return this.readDirectory( this.dir )
            .then( files => this.files = files )
            .then( () => this )
            .catch( console.error );
    }
    
    run()
    {
        if( !this.files ) {
            throw new Error( 'Argument Error - must initialize Whatodo first' );
        }
        
        return this.files.filter(
            item => {
                item.todos = this.searchFile( item.fname );
                return item.todos.length ? item : false;
            }
        );
    }
    
    searchFile( file )
    {
        return todo.searchFile( file );
    }
    
    fstats( fname )
    {
        return new Promise(
            ( res, rej ) => stat( fname,
                ( e, d ) => e ?
                    rej( e ) :
                    res( { fname, isDirectory: d.isDirectory(), size: d.size } )
            )
        );
    }
    
    readDirectory( dir, files = [] )
    {
        return new Promise(
            ( res, rej ) => readdir( dir,
                ( e, d ) => e ? rej( e ) : Promise.all(
                    d.map( fn => {
                        if( fn.match( this.ignoreRx ) ) {
                            return;
                        }
                        
                        fn = join( dir, fn );
                        
                        return this.fstats( fn )
                            .then( info => info.isDirectory ?
                                this.readDirectory( info.fname, files ) :
                                files.push( info )
                            )
                            .catch( rej );
                    } )
                )
                    .then( () => res( files ) )
                    .catch( rej )
            )
        );
    }
}

module.exports = Whatodo;
