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

## Error Handling and Logging

During initialization and other critical parts of the application, we use the following error handling and logging approach:

1. Use SDL3's error handling functions to check for errors (e.g., `SDL_Init`, `SDL_CreateWindow`).
2. Log error messages using SDL3's `SDL_GetError` function.
3. Use `spdlog` for logging with different log levels (debug, info, warning, error, critical).
4. Configure log file rotation using `spdlog::rotating_logger_mt`.
5. Display appropriate error messages to the user, indicating that the initialization has failed.
6. Ensure that any resources allocated during initialization are properly cleaned up in case of an error.
7. Exit the application gracefully if an error occurs during initialization.

### Logging System

We use `spdlog` for logging in this project. The logging system is configured with different log levels (debug, info, warning, error, critical) and log file rotation. Here is how to use the logging system:

1. Include the `spdlog` header in your source files to use the logging functionality.
   ```cpp
   #include <spdlog/spdlog.h>
   ```

2. Create a logger using `spdlog::rotating_logger_mt` and configure log file rotation.
   ```cpp
   auto logger = spdlog::rotating_logger_mt("logger", "logs/logfile.log", 1048576 * 5, 3);
   ```

3. Set the desired log level using the `set_level` method.
   ```cpp
   logger->set_level(spdlog::level::debug);
   ```

4. Use the logger to log messages at different levels.
   ```cpp
   logger->debug("This is a debug message");
   logger->info("This is an info message");
   logger->warn("This is a warning message");
   logger->error("This is an error message");
   logger->critical("This is a critical message");
   ```

5. Ensure that the `CMakeLists.txt` file is configured to include and link `spdlog` for logging.
