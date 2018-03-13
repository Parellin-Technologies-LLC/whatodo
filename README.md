<p align="center">
    <a href="#">
        <img width="242px"
             height="72px"
             alt="whatodo"
             src="./todo.png" />
    </a>
</p>

# whatodo

[https://whatodo.io](https://whatodo.io) (coming soon)

A TODO collector, organizer, and task manager

Do you write `TODO` comments all over your code?

I certainly do.

Wouldn't it be nice to pull those TODOs out and have priorities, links to where to your code, all ready to go?

### Install

`npm i -g whatodo`

### Usage

List your TODO comments throughout your code

When ready, run the `todo` cli to collect your comments:

```
  
  Whatodo v0.2.1
  
  Whatodo is a NPM Module designed to quickly and efficiently extract your
  TODO comments from your code and organize them in a simple & readable format
  
  Usage:
  
    todo [input file/folder] [output file] [...options]
  
  Options:
  
    -h, --help      show this help menu
    -v, --version   show package version
    -i, --input     directory or file to check TODOs  (default: ./)
    -o, --output    file to save TODOs                (default: stdout)
    -f, --format    format to save TODOs              (default: STDOUT, "JSON")
    -p, --pattern   pattern to capture TODOs          (default: "\/\/ ?TODO:?:?:? ?")
    -m, --maximum   maximum file size allowed         (default: "1 MB", 512KiB)
```

List priorities:

- `// TODO: low priority`
- `// TODO:: mid priority`
- `// TODO::: high priority`

When you want to collect your TODOs run:

`todo [input file/folder] [output file] [...options]`

Output:

```
[
    {
        "fname": "/whatodo/index.js",
        "timing": "156.803 μs",
        "size": 155,
        "todos": [
            {
                "priority": "LOW",
                "line": 14,
                "position": 3,
                "comment": "low priority"
            },
            {
                "priority": "MID",
                "line": 15,
                "position": 3,
                "comment": "mid priority"
            },
            {
                "priority": "HIGH",
                "line": 16,
                "position": 3,
                "comment": "high priority"
            }
        ]
    }
]
```