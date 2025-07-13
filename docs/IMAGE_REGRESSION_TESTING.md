# Image Regression Testing

This document describes the image regression testing system for the platformer_prototype project. This system automatically captures screenshots of the game and compares them against reference images to detect visual regressions.

## Overview

The image regression testing system:
- Captures screenshots of the game running in different scenarios
- Compares current screenshots with reference images
- Reports pixel-level differences
- Generates HTML and JSON reports
- Integrates with CI/CD pipeline

## Components

### 1. Screenshot Capture (C++)
- `src/ScreenshotCapture.h/cpp` - Adds screenshot functionality to the game
- Command-line options:
  - `--screenshot <filename>` - Captures a screenshot to the specified file
  - `--screenshot-delay <ms>` - Delay before taking screenshot (default: 1000ms)
  - `--testMode` - Enables automated test mode (exits after screenshot)

### 2. Image Comparison (Node.js)
- `tests/image-regression/image-test.js` - Main test runner
- Uses [pixelmatch](https://github.com/mapbox/pixelmatch) for pixel-level comparison
- Uses [Jimp](https://github.com/jimp-dev/jimp) for image processing

### 3. Test Infrastructure
- Runs tests in headless mode using Xvfb
- Configurable tolerance levels for comparison
- Automatic reference image generation

## Usage

### Running Tests Locally

```bash
# Build the project first
mkdir build && cd build
cmake .. && make -j4
cd ..

# Install Node.js dependencies
npm install

# Run image regression tests
npm run test:image

# Update reference images (when you want to accept current images as new baselines)
npm run test:image:update
```

### Test Scenarios

The following test scenarios are automatically executed:

1. **main-menu** - Initial game state
2. **level-start** - Game with test level loaded
3. **character-spawn** - Character spawned in level
4. **developer-mode** - Developer mode enabled

### Adding New Tests

To add a new test scenario, edit `tests/image-regression/image-test.js` and add to the `tests` array:

```javascript
const tests = [
    // ... existing tests
    { name: 'new-scenario', args: ['--custom-arg', 'value'] },
];
```

### Configuration

Edit the `CONFIG` object in `image-test.js` to adjust:

- `tolerance` - Pixel matching threshold (0.0-1.0)
- `maxAllowedDiffPercentage` - Maximum percentage of different pixels allowed
- `screenshotDelay` - Delay before capturing screenshots

## CI/CD Integration

The image regression tests run automatically on:
- Push to main/develop branches
- Pull requests to main/develop branches

### GitHub Actions Workflow

See `.github/workflows/image-regression.yml` for the complete CI configuration.

The workflow:
1. Builds the C++ application
2. Sets up Node.js environment
3. Runs image regression tests
4. Uploads test results as artifacts

### Artifacts

- **image-regression-results** - All screenshots and diff images
- **image-regression-report** - HTML test report

## Directory Structure

```
tests/image-regression/
├── image-test.js          # Main test runner
├── output/                # Generated screenshots (ignored by git)
├── reference/             # Reference images (committed to git)
└── diff/                  # Difference images (ignored by git)
```

## Troubleshooting

### Common Issues

1. **"No available video device"**
   - Ensure Xvfb is installed: `sudo apt-get install xvfb`
   - Tests should run with `xvfb-run`

2. **"Failed to save PNG"**
   - Check directory permissions
   - Ensure output directory exists

3. **High pixel differences**
   - Check if game assets have changed
   - Verify consistent rendering between environments
   - Consider updating reference images if changes are intentional

### Debugging

Enable verbose logging by setting log level in the game:
```bash
./build/platformer_prototype --screenshot test --testMode --verbose
```

View detailed test output:
```bash
npm run test:image 2>&1 | tee test-output.log
```

## Maintenance

### Updating Reference Images

When game visuals are intentionally changed:

1. Run tests to see current differences
2. Review generated screenshots in `tests/image-regression/output/`
3. If changes are acceptable, update references:
   ```bash
   npm run test:image:update
   ```
4. Commit updated reference images

### Performance Considerations

- Each test captures a full 800x600 screenshot (~300KB PNG)
- Tests run sequentially to avoid interference
- Total test time: ~10-15 seconds for 4 scenarios

## Integration with Development Workflow

### Before Committing Changes

```bash
# Build and test
npm run build
npm run test:image
```

### Reviewing Visual Changes in PRs

1. Check CI artifacts for screenshots
2. Download and compare diff images
3. Ensure changes are intentional
4. Update reference images if needed