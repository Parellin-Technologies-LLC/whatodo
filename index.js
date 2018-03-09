/** ****************************************************************************************************
 * File: index.js
 * Project: whatodo
 * @author Nick Soggin <iSkore@users.noreply.github.com> on 01-Feb-2018
 *******************************************************************************************************/
'use strict';

const
	{ readdir, stat, writeFile }  = require( 'fs' ),
	{ resolve, join }             = require( 'path' ),
	todo                          = require( 'bindings' )( 'todo' ),
	{ convertHighResolutionTime } = require( './utils' );

// TODO:: write docs, installation, usage, etc
// TODO: note syntax and priority specification for different priority levels
// TODO: allow progress bar in CLI
// TODO::: allow single file or directory TODO search
class Whatodo
{
	constructor( opts )
	{
		opts = opts || {};
		
		this.initialized  = false;
		this.dir          = resolve( opts.dir || process.cwd() );
		this.ignore       = opts.ignore || [ 'node_modules', '.git', '.idea', 'docs', 'build' ];
		this.ignoreRx     = new RegExp( `^${this.ignore.join( '$|^' )}$` );
		this.ignoreExts   = opts.ignoreExts || [ 'json', 'html', 'css', 'md' ];
		this.ignoreExtsRx = new RegExp( `\\.(${this.ignoreExts.join( '|' )})+$` );
		
		this.todoPattern = opts.todoPattern || '\\/\\/ ?TEST:?:?:? ?';
		
		this.outputFile   = opts.outputFile ? resolve( opts.outputFile ) : null;
		this.outputFormat = opts.outputFormat || Whatodo.JSON;
		this.opts         = {
			dir: this.dir,
			todoPattern: this.todoPattern
		};
	}
	
	initialize()
	{
		return this.readDirectory( this.dir )
			.then( files => this.files = files )
			.then( () => this.initialized = true )
			.then( () => this )
			.catch( console.error );
	}
	
	run()
	{
		this.timeStart = process.hrtime();
		
		if( !this.initialized ) {
			throw new Error( 'Error - Whatodo must be initialized before calling run' );
		}
		
		return new Promise(
			( res, rej ) => {
				if( !this.files || !this.files.length ) {
					rej( 'Argument Error - No files found (initialize Whatodo first)' );
				}
				
				this.todos = this.files.filter(
					item => {
						const result = this.searchFile( item.fname, this.opts );
						return result.todos.length ? result : false;
					}
				);
				
				this.todos.filesSearched = this.files.length;
				
				this.timeEnd = process.hrtime( this.timeStart );
				this.timing  = convertHighResolutionTime( this.timeEnd, 2 );
				res( this );
			}
		);
	}
	
	save()
	{
		if( !this.outputFile ) {
			return console.log( this.todos );
			// return Promise.resolve( console.log( this.todos ) );
		}
		
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
	
	searchFile( file, opts = {} ) {
		return Whatodo.searchFile( file, opts );
	}
	
	static searchFile( file, opts = {} )
	{
		return todo.searchFile( file, opts );
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
