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

## Continuous Integration (CI) Setup

We have set up continuous integration using Github Actions for CMake builds on multiple platforms (Ubuntu, macOS, Windows). The CI workflow is defined in the `.github/workflows/ci.yml` file.

### Viewing Build Results

To view the build results, follow these steps:

1. Go to the repository on Github.
2. Click on the "Actions" tab.
3. You will see a list of workflow runs. Click on the workflow run you are interested in.
4. You can view the details of the build, including the build logs and test results.

### Downloading Build Artifacts

To download the build artifacts, follow these steps:

1. Go to the repository on Github.
2. Click on the "Actions" tab.
3. You will see a list of workflow runs. Click on the workflow run you are interested in.
4. In the workflow run details, you will find a section for "Artifacts". Click on the artifact you want to download.
5. The artifact will be downloaded as a zip file. Extract the zip file to access the build artifacts.
