<p align="center">
    <a href="#">
        <img width="242px"
             height="72px"
             alt="xterrainlogo"
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
  Whatodo
  
  Whatodo is a NPM Module designed to quickly and efficiently extract your
  TODO comments from your code and organize them in a simple & readable format
  
  Options:
  
    -h, --help       show this help menu
    -v, --version    show package version
    -d, --dir        directory to check TODOs
    -o, --output     output file to save TODOs
```

List priorities:

- `// TODO: low priority`
- `// TODO:: mid priority`
- `// TODO::: high priority`

When you want to collect your TODOs run:

`todo [directory] [..options]`

Output:

```
[
    {
        "fname": "/whatodo/README.md",
        "isDirectory": false,
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
        ],
        "timing": "156.803 μs"
    }
]
```