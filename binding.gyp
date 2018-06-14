{
    "targets": [
        {
            "target_name": "whatodo",
            "sources": [ "src/todo.cc" ],
            "include_dirs": [ "<!@(node -p \"require('node-addon-api').include\")" ],
            "dependencies": [ "<!@(node -p \"require('node-addon-api').gyp\")" ]
        }
    ]
}