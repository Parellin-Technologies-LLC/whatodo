/** ****************************************************************************************************
 * File: index.js
 * Project: whatodo
 * @author Nick Soggin <iSkore@users.noreply.github.com> on 01-Feb-2018
 *******************************************************************************************************/
'use strict';

const
    { readdir, stat, writeFile } = require( 'fs' ),
    { resolve, join }            = require( 'path' ),
    todo                         = require( 'bindings' )( 'todo' );

// TODO::: make CLI input
// TODO:: write docs, installation, usage, etc
// TODO: note syntax and priority specification for different priority levels
// TODO write more tests
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
        
        this.todos = this.files.filter(
            item => {
                item.todos  = this.searchFile( item.fname );
                item.timing = item.todos.time;
                
                if( item.timing < 1000 ) {
                    item.timing = `${item.todos.time} ns`;
                } else if( item.timing < 1000000 ) {
                    item.timing = `${item.todos.time / 1e3} μs`;
                } else if( item.timing < 1000000000 ) {
                    item.timing = `${item.todos.time / 1e6} ms`;
                } else if( item.timing < 1000000000000 ) {
                    item.timing = `${item.todos.time / 1e9} s`;
                }
                
                return item.todos.length ? item : false;
            }
        );
        
        return this;
    }
    
    save( fn )
    {
        fn = resolve( fn );
        return new Promise(
            ( res, rej ) => writeFile(
                fn,
                JSON.stringify( this.todos, null, 4 ),
                e => e ? rej( e ) : res( `${fn} SAVED` )
            )
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
                            .then(
                                info => info.isDirectory ?
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
