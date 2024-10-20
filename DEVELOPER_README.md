# Developer Readme

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

1. Use SDL2's error handling functions to check for errors (e.g., `SDL_Init`, `SDL_CreateWindow`).
2. Log error messages using SDL2's `SDL_GetError` function.
3. Display appropriate error messages to the user, indicating that the initialization has failed.
4. Ensure that any resources allocated during initialization are properly cleaned up in case of an error.
5. Exit the application gracefully if an error occurs during initialization.

## Application Usage

For information on what the application does and how to use it, please refer to the `README.md` file.

## Updating Submodules

To ensure that submodules are updated, follow these steps:

1. Add the `submodules: true` option to the `actions/checkout` action in your GitHub Actions workflow file.
2. Ensure that the `.gitmodules` file is properly configured with the correct submodule URLs and paths.
3. Verify that the submodules are correctly initialized and updated in your local repository by running the following commands:
   ```
   git submodule init
   git submodule update
   ```
4. If you encounter any issues with submodules, check the `.git/config` file to ensure that the submodule URLs and paths are correctly configured.

## Continuous Integration using GitHub Actions

We have set up continuous integration using GitHub Actions for our commits. The workflow is configured to use `actions/checkout@v3` and `actions/upload-artifact@v4` with submodules updated. The workflow includes steps to build the project using CMake and run tests on multiple platforms (e.g., Ubuntu, macOS, Windows).

The workflow file `.github/workflows/ci.yml` includes the following steps:

1. Checkout the repository with `submodules: true` option.
2. Set up the required dependencies (e.g., SDL2, CMake).
3. Build the project using CMake.
4. Run tests to ensure the build is successful.
5. Upload build artifacts using `actions/upload-artifact@v4`.

The workflow is triggered on push and pull request events to the main branch.
