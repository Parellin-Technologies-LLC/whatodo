{
    "targets": [
        {
            "target_name": "todo",
            "sources": [ "todo.cc" ],
            "defines": [ "V8_DEPRECATION_WARNINGS=1" ],
            "include_dirs" : [
                "<!@(node -p \"require( 'node-addon-api' ).include\")"
            ],
            "defines": [ "NAPI_DISABLE_CPP_EXCEPTIONS" ]
        }
    ]
}