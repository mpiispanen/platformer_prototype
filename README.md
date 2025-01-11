# Platformer Prototype

[![Build Linux](https://github.com/mpiispanen/platformer_prototype/actions/workflows/ci-linux.yml/badge.svg)](https://github.com/mpiispanen/platformer_prototype/actions/workflows/ci-linux.yml)
[![Build Windows](https://github.com/mpiispanen/platformer_prototype/actions/workflows/ci-windows.yml/badge.svg)](https://github.com/mpiispanen/platformer_prototype/actions/workflows/ci-windows.yml)
[![Static Analysis](https://github.com/mpiispanen/platformer_prototype/actions/workflows/static-analysis.yml/badge.svg)](https://github.com/mpiispanen/platformer_prototype/actions/workflows/static-analysis.yml)

## Description

Platformer Prototype is a simple platformer game built using SDL3. The game allows players to control a character and navigate through a 2D environment. The purpose of this project is to demonstrate the use of SDL3 for game development and provide a starting point for building more complex games.

## How to Run

You can run the application by executing the following command:

```sh
./platformer_prototype --width 1024 --height 768 --fullscreen
```

## Building the Project

To build the project, you need to have the following dependencies installed:

- A C++ compiler (e.g., g++ or clang++)
- CMake

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

1. Use SDL3's error handling functions to check for errors (e.g., `SDL_Init`, `SDL_CreateWindow`).
2. Log error messages using SDL3's `SDL_GetError` function.
3. Display appropriate error messages to the user, indicating that the initialization has failed.
4. Ensure that any resources allocated during initialization are properly cleaned up in case of an error.
5. Exit the application gracefully if an error occurs during initialization.

If you encounter any issues while running the application, please check the following:

1. Ensure that SDL3 is installed correctly on your system.
2. Verify that your system meets the minimum requirements for running the application.
3. Check the console output for any error messages and follow the instructions provided.
4. If the issue persists, please open an issue on the project's GitHub repository with a detailed description of the problem.
