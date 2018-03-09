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
		
		this.initialized = false;
		this.input       = resolve( opts.input || process.cwd() );
		
		this.ignore     = opts.ignore || [ 'node_modules', '.git', '.idea', 'docs', 'build' ];
		this.ignoreExts = opts.ignoreExts || [ 'json', 'html', 'css', 'md' ];
		
		this.ignoreRx     = new RegExp( `^${this.ignore.join( '$|^' )}$` );
		this.ignoreExtsRx = new RegExp( `\\.(${this.ignoreExts.join( '|' )})+$` );
		
		this.todoPattern = opts.todoPattern || '\\/\\/ ?TODO:?:?:? ?';
		
		this.outputFile   = opts.outputFile ? resolve( opts.outputFile ) : null;
		this.outputFormat = opts.outputFormat || Whatodo.JSON;
		this.opts         = {
			input: this.input,
			todoPattern: this.todoPattern
		};
	}
	
	initialize()
	{
		return this.fstats( this.input )
			.then( d => {
				if( d.isDirectory ) {
					return this.readDirectory( this.input );
				} else {
					return [ d ];
				}
			} )
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
				
				this.todos = this.files.reduce(
					( r, item ) => {
						const result = this.searchFile( item.file, this.opts );
						result.size  = item.size;
						
						if( result.todos.length ) {
							r.push( result );
						}
						
						return r;
					}, []
				);
				
				this.filesSearched = this.files.length;
				this.timeEnd       = process.hrtime( this.timeStart );
				this.totalTime     = convertHighResolutionTime( this.timeEnd, 2 );
				
				res( this );
			}
		);
	}
	
	searchFile( file, opts = {} ) {
		return Whatodo.searchFile( file, opts );
	}
	
	static searchFile( file, opts = {} )
	{
		file = resolve( file );
		return todo.searchFile( file, opts );
	}
	
	removeTodo( file, line )
	{
		return Whatodo.removeTodo( file, line );
	}
	
	static removeTodo( file, line )
	{
		file = resolve( file );
		return todo.removeTodo( file, line );
	}
	
	fstats( file )
	{
		return new Promise(
			( res, rej ) => stat( file,
				( e, d ) => e ?
					rej( e ) :
					res( { file, isDirectory: d.isDirectory(), size: d.size } )
			)
		);
	}
	
	readDirectory( input, files = [] )
	{
		return new Promise(
			( res, rej ) => readdir( input,
				( e, d ) => e ? rej( e ) : Promise.all(
					d.map( fn => {
						if( fn.match( this.ignoreRx ) || fn.match( this.ignoreExtsRx ) ) {
							return;
						}
						
						fn = join( input, fn );
						
						return this.fstats( fn )
							.then(
								info => info.isDirectory ?
									this.readDirectory( info.file, files ) :
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
	
	getTodos()
	{
		return this.todos;
	}
	
	print()
	{
		return process.stdout.write( JSON.stringify( this.getTodos(), null, 4 ) );
	}
	
	save( fn )
	{
		return Promise.resolve( this.outputFile || resolve( fn ) )
			.catch( () => console.log( 'Argument Error - must specify outputFile' ) )
			.then( d => this.outputFile = d )
			.then( () => new Promise(
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
			) )
			.catch( console.error );
	}
}

Whatodo.XML  = 'XML';
Whatodo.JSON = 'JSON';
Whatodo.YAML = 'YAML';

module.exports = Whatodo;
