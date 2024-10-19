{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "C++: Launch Debug",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/debug/platformer_prototype",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "lldb",
            "preLaunchTask": "Build Debug",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for lldb",
                    "text": "settings set target.inline-breakpoint-strategy always",
                    "ignoreFailures": true
                }
            ],
            "miDebuggerPath": "/Users/matpii01/.vscode/extensions/ms-vscode.cpptools-1.22.9-darwin-x64/debugAdapters/lldb-mi/bin/lldb-mi",
            "logging": {
                "engineLogging": true
            }
        },
        {
            "name": "C++: Launch Release",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/release/platformer_prototype",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "lldb",
            "preLaunchTask": "Build Release",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for lldb",
                    "text": "settings set target.inline-breakpoint-strategy always",
                    "ignoreFailures": true
                }
            ],
            "miDebuggerPath": "/usr/bin/lldb",
            "logging": {
                "engineLogging": true
            }
        }
    ]
}