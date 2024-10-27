# Developer Readme

## Building the Project

To build the project, ensure you have the following dependencies installed:

- A C++ compiler (e.g., g++ or clang++)
- CMake

1. Create a build directory:
   ```sh
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

## Code Structure

The project is organized as follows:

- `src/`: Contains the source code files.
  - `main.cpp`: The main entry point of the application, including SDL2 initialization, argument parsing, and the game loop.

## Contributing

We welcome contributions to the project! Here are some guidelines for contributing:

1. Fork the repository and create a new branch for your feature or bugfix.
2. Write clean, readable, and well-documented code.
3. Follow the existing coding standards and style.
4. Ensure that your changes do not break the existing functionality.
5. Submit a pull request with a clear description of your changes.

## Error Handling

During initialization and other critical parts of the application, we use the following error handling approach:

1. Use SDL3's error handling functions to check for errors (e.g., `SDL_Init`, `SDL_CreateWindow`).
2. Log error messages using SDL3's `SDL_GetError` function.
3. Display appropriate error messages to the user, indicating that the initialization has failed.
4. Ensure that any resources allocated during initialization are properly cleaned up in case of an error.
5. Exit the application gracefully if an error occurs during initialization.

## Application Usage

For information on what the application does and how to use it, please refer to the `README.md` file.
