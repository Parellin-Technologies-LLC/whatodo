<p align="center">
    <a href="#">
        <img width="242px"
             height="72px"
             alt="whatodo"
             src="./todo.png" />
    </a>
</p>

<h1><a name="top">whatodo</a></h1>

- <a href="#install">Install</a>
- <a href="#apiusage">API Usage</a>
- <a href="#cliusage">CLI Usage</a>

[https://whatodo.io](https://whatodo.io) (coming soon)

A TODO collector, organizer, and task manager

Do you write `TODO` comments all over your code?

I certainly do.

Wouldn't it be nice to pull those TODOs out and have priorities, links to where to your code, all ready to go?

### <a name="install" href="#top">Install</a>

`npm i -g whatodo`

List priorities:

- `// TODO: low priority`
- `// TODO:: mid priority`
- `// TODO::: high priority`

NPM Problems?

Try this: `sudo npm install -g whatodo --unsafe-perm --allow-root`

### <a name="apiusage" href="#top">API Usage</a>

Options:
- `input` - input file or directory
- `ignore` - array of files or directories to ignore
- `ignoreExts` - array of extensions to ignore
- `todoPattern` - regex pattern to search for (default: `'\\/\\/ ?TODO:?:?:? ?'`)
- `outputFile` - specify the file where the todos are saved
- `outputFormat` - format to export todos (`JSON`, `STDOUT`)
- `maximumFileSize` - maximum file size to search (`1 KiB`, `1 Megabyte`, `2 MB`)

```
'use strict';

const Whatodo = require( 'whatodo' );

new Whatodo( {
    input: 'index.js',
    ignore: [ 'node_modules', '.git', '.idea', 'docs', 'build' ],
    ignoreExts: [ 'json', 'html', 'css', 'md' ],
    todoPattern: '\\/\\/ ?TODO:?:?:? ?',
    outputFile: 'todos.json',
    outputFormat: 'JSON',
    maximumFileSize: '1 MiB'
} )
    .initialize()
	.then( inst => inst.run() )
	.then( inst => inst.save() )
	.catch( console.error );
```

### <a name="cliusage" href="#top">CLI Usage</a>

Write your TODO comments throughout your code

When ready, run the `todo` cli to collect your comments:

### `todo -h`
```
  
  Whatodo v0.3.0
  
  Whatodo is a NPM Module designed to quickly and efficiently extract your
  TODO comments from your code and organize them in a simple & readable format
  
  Usage:
  
    todo [input file/folder] [...options]
  
  Options:
  
    -h, --help      show this help menu
    -v, --version   show package version
    -i, --input     directory or file to check TODOs  (default: ./)
    -o, --output    file to save TODOs                (default: stdout)
    -f, --format    format to save TODOs              (default: STDOUT, "JSON")
    -p, --pattern   pattern to capture TODOs          (default: "\/\/ ?TODO:?:?:? ?")
    -m, --maximum   maximum file size allowed         (default: "1 MB", 512KiB)
  
```

### `todo ./ -f JSON -o todos.json -m 1KiB -p "\/\/ ?TEST:?:?:? ?"`

Specify:
- JSON formatted output
- Output file `todos.json`
- Set a maximum search of 1 Kibibyte files
- TODO search regex `"\/\/ ?TEST:?:?:? ?"`

Creates a `todos.json` file:

```
[
    {
        "file": "whatodo/test/test.cpp",
        "timing": "120.263 μs",
        "todos": [
            {
                "priority": "LOW",
                "line": 1,
                "position": 0,
                "comment": "todo low priority"
            },
            {
                "priority": "MID",
                "line": 2,
                "position": 0,
                "comment": "todo mid priority"
            },
            {
                "priority": "HIGH",
                "line": 3,
                "position": 0,
                "comment": "todo high priority"
            }
        ],
        "size": 182
    }
]
```
