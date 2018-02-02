# whatodo
###[https://whatodo.io](https://whatodo.io) (coming soon)

A TODO collector, organizer, and task manager

Do you write `TODO` comments all over your code?

I certainly do.

Wouldn't it be nice to pull those TODOs out and have priorities, links to where to your code, all ready to go?

List priorities:

- `// TODO: low priority`
- `// TODO:: mid priority`
- `// TODO::: high priority`

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