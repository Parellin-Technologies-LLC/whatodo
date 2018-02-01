{
    "targets": [
        {
            "target_name": "todo",
            "sources": [ "todo.cc" ],
            "include_dirs" : [
                "<!(node -e \"require('nan')\")"
            ]
        }
    ]
}