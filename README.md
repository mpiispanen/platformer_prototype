# Platformer Prototype

## Description

Platformer Prototype is a simple platformer game built using SDL2. The game allows players to control a character and navigate through a 2D environment. The purpose of this project is to demonstrate the use of SDL2 for game development and provide a starting point for building more complex games.

## How to Run

To run the application, you need to have SDL2 installed on your system. You can download SDL2 from the official website: https://www.libsdl.org/download-2.0.php

Once SDL2 is installed, you can run the application by executing the following command:

```
./platformer_prototype
```

## Building the Project

To build the project, you need to have the following dependencies installed:

- SDL2
- A C++ compiler (e.g., g++ or clang++)
- CMake (optional, for building the project with CMake)

### Building with CMake

1. Create a build directory:
   ```
   mkdir build
   cd build
   ```

2. Run CMake to generate the build files:
   ```
   cmake ..
   ```

3. Build the project:
   ```
   make
   ```

### Building without CMake

1. Compile the source files using a C++ compiler:
   ```
   g++ -o platformer_prototype src/main.cpp -lSDL2
   ```

## Command-Line Arguments

The application supports the following command-line arguments:

- `--width` or `-w`: Set the window width (default: 800)
- `--height` or `-h`: Set the window height (default: 600)
- `--fullscreen` or `-f`: Enable fullscreen mode (default: false)
- `--help`: Print help message

Example usage:
```
./platformer_prototype --width 1024 --height 768 --fullscreen
```

## Error Handling and Troubleshooting

During initialization and other critical parts of the application, we use the following error handling approach:

1. Use SDL2's error handling functions to check for errors (e.g., `SDL_Init`, `SDL_CreateWindow`).
2. Log error messages using SDL2's `SDL_GetError` function.
3. Display appropriate error messages to the user, indicating that the initialization has failed.
4. Ensure that any resources allocated during initialization are properly cleaned up in case of an error.
5. Exit the application gracefully if an error occurs during initialization.

If you encounter any issues while running the application, please check the following:

1. Ensure that SDL2 is installed correctly on your system.
2. Verify that your system meets the minimum requirements for running the application.
3. Check the console output for any error messages and follow the instructions provided.
4. If the issue persists, please open an issue on the project's GitHub repository with a detailed description of the problem.
