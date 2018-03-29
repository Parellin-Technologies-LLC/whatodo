/** ****************************************************************************************************
 * File: index.js
 * Project: whatodo
 * @author Nick Soggin <iSkore@users.noreply.github.com> on 01-Feb-2018
 *******************************************************************************************************/
'use strict';

const
	style                        = require( 'ansi-styles' ),
	{ readdir, stat, writeFile } = require( 'fs' ),
	{ resolve, join }            = require( 'path' ),
	todo                         = require( 'bindings' )( 'whatodo' ),
	{
		convertHighResolutionTime,
		bytesToSize,
		sizeToBytes
	}                            = require( './utils' );

// TODO:: write API docs for removing a TODO
// TODO: note syntax and priority specification for different priority levels
class Whatodo
{
	constructor( opts )
	{
		opts = opts || {};
		
		this.initialized = false;
		
		this.input = resolve( opts.input || process.cwd() );
		
		this.ignore     = opts.ignore || [ 'node_modules', '.git', '.idea', 'docs', 'build', '9.9.0' ];
		this.ignoreExts = opts.ignoreExts || [ 'json', 'html', 'css', 'md' ];
		
		this.ignoreRx     = new RegExp( `^${this.ignore.join( '$|^' )}$` );
		this.ignoreExtsRx = new RegExp( `\\.(${this.ignoreExts.join( '|' )})+$` );
		
		this.todoPattern = opts.todoPattern || '\\/\\/ ?TODO:?:?:? ?';
		
		this.outputFile   = opts.outputFile ? resolve( opts.outputFile ) : null;
		this.outputFormat = opts.outputFormat || Whatodo.FORMAT.JSON;
		
		this.maximumFileSize = sizeToBytes( opts.maximumFileSize || '1 MiB' );
	}
	
	initialize()
	{
		return this.fstats( this.input )
			.then( d => d.isDirectory ? this.readDirectory( this.input ) : [ d ] )
			.then( files => this.files = files )
			.then( () => this.initialized = true )
			.then( () => this );
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
						if( item.skip ) {
							r.push( item );
							return r;
						}
						
						const
							result = this.searchFile( item.file, {
								input: this.input,
								todoPattern: this.todoPattern
							} );
						
						if( result.todos.length ) {
							result.size = item.size;
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
									files.push(
										info.size <= this.maximumFileSize ? info : ( info.skip = true, info )
									)
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
	
	stylePriorityColor( priority, msg )
	{
		return priority === 'HIGH' ? `${style.redBright.open}${msg}${style.redBright.close}` :
			priority === 'MID' ? `${style.yellowBright.open}${msg}${style.yellowBright.close}` :
				priority === 'LOW' ? `${style.greenBright.open}${msg}${style.greenBright.close}` :
					msg;
	}
	
	convertToStdoutFormat()
	{
		const todos = this.getTodos();
		
		return todos.reduce(
			( r, item ) => {
				r += style.bgWhiteBright.open + style.blue.open;
				r += `${item.file}  (${item.timing} - ${item.size} bytes)`;
				r += style.blue.close + style.bgWhiteBright.close;
				r += '\n';
				
				if( item.skip ) {
					r += `    Item Skipped - Maximum File Size Exceeded ${bytesToSize( this.maximumFileSize )}\n`;
				} else {
					item.todos.forEach(
						todo => {
							const priority = todo.priority;
							
							let msg = '';
							
							msg += '    ';
							msg += `[${priority}]`;
							msg += ' '.repeat( 5 - priority.length );
							msg += `line: ${todo.line}`;
							msg += ` - ${todo.comment}`;
							
							r += `${this.stylePriorityColor( priority, msg )}\n`;
						}
					);
				}
				
				return `${r}\n`;
			}, ''
		);
	}
	
	printStdout()
	{
		return process.stdout.write( this.convertToStdoutFormat() );
	}
	
	printJSON()
	{
		return process.stdout.write( JSON.stringify( this.getTodos(), null, 4 ) );
	}
	
	print()
	{
		if( this.outputFormat === Whatodo.FORMAT.JSON ) {
			return this.printJSON();
		} else if( this.outputFormat === Whatodo.FORMAT.STDOUT ) {
			return this.printStdout();
		} else {
			return console.error( `Format: ${this.outputFormat} not supported yet` );
		}
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
					
					let output = this.todos;
					
					if( this.outputFormat === Whatodo.FORMAT.JSON ) {
						output = JSON.stringify( this.todos, null, 4 );
					} else if( this.outputFormat === Whatodo.FORMAT.STDOUT ) {
						output = this.convertToStdoutFormat();
					} else {
						return console.error( `Format: ${this.outputFormat} not supported yet` );
					}
					
					writeFile( this.outputFile, output,
						e => e ? rej( e ) : res( this )
					);
				}
			) );
	}
}

Whatodo.FORMAT = {
	STDOUT: 'STDOUT',
	JSON: 'JSON',
	XML: 'XML',   // NOT SUPPORTED YET
	YAML: 'YAML', // NOT SUPPORTED YET
	HTML: 'HTML'  // NOT SUPPORTED YET
};

module.exports = Whatodo;
