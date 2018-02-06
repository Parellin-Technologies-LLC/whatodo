/** ****************************************************************************************************
 * File: index.js
 * Project: whatodo
 * @author Nick Soggin <iSkore@users.noreply.github.com> on 01-Feb-2018
 *******************************************************************************************************/
'use strict';

const
    { readdir, stat, writeFile } = require( 'fs' ),
    { resolve, join }            = require( 'path' ),
    // todo                         = require( 'bindings' )( 'todo' );
    todo                         = require( './build/Release/todo' );

// TODO:: write docs, installation, usage, etc
// TODO: note syntax and priority specification for different priority levels
// TODO: allow progress bar in CLI
class Whatodo
{
    constructor( opts )
    {
        opts = opts || {};
        
        this.dir          = resolve( opts.dir || process.cwd() );
        this.ignore       = opts.ignore || [ 'node_modules', '.git', '.idea' ];
        this.ignoreRx     = new RegExp( `^${this.ignore.join( '$|^' )}$` );
        this.ignoreExts   = opts.ignoreExts || [ 'json', 'html', 'css', 'md' ];
        this.ignoreExtsRx = new RegExp( `\\.(${this.ignoreExts.join( '|' )})+$` );
        this.todoFormat   = opts.todoFormat || '\\/\\/ ?TODO:?:?:?';
        this.outputFile   = resolve( opts.outputFile || './TODOS.json' );
        this.outputFormat = opts.outputFormat || Whatodo.JSON;
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
        return new Promise(
            ( res, rej ) => {
                if( !this.files || !this.files.length ) {
                    rej( 'Argument Error - No files found (initialize Whatodo first)' );
                }
                
                this.todos = this.files.filter(
                    item => {
                        item.todos  = this.searchFile( item.fname );
                        item.timing = item.todos.time;
                        
                        item.timing = item.timing < 1000 ? `${item.todos.time} ns` :
                            item.timing < 1000000 ? `${item.todos.time / 1e3} μs` :
                                item.timing < 1000000000 ? `${item.todos.time / 1e6} ms` :
                                    `${item.todos.time / 1e9} s`;
                        
                        return item.todos.length ? item : false;
                    }
                );
                
                res( this );
            }
        );
    }
    
    save( fn )
    {
        this.outputFile = this.outputFile || resolve( fn );
        return new Promise(
            ( res, rej ) => {
                if( !this.todos.length ) {
                    return rej( `No TODOs found in ${this.outputFile}` );
                }
                
                let format = this.todos;
                
                if( this.outputFormat === Whatodo.JSON ) {
                    format = JSON.stringify( this.todos, null, 4 );
                } else {
                    return console.error( `Format: ${this.outputFormat} not supported yet` );
                }
                
                writeFile( this.outputFile, format,
                    e => e ? rej( e ) : res( `${this.outputFile} SAVED` )
                );
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
                        if( fn.match( this.ignoreRx ) || fn.match( this.ignoreExtsRx ) ) {
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

Whatodo.XML  = 'XML';
Whatodo.JSON = 'JSON';
Whatodo.YAML = 'YAML';

module.exports = Whatodo;
