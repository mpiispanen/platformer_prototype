#!/usr/bin/env node

/**
 * Image regression testing for platformer_prototype
 * 
 * This script captures screenshots from the running game and compares them
 * against reference images to detect visual regressions.
 */

const fs = require('fs').promises;
const path = require('path');
const { exec } = require('child_process');
const { promisify } = require('util');
const { Jimp } = require('jimp');
const pixelmatch = require('pixelmatch').default;

const execAsync = promisify(exec);

const CONFIG = {
    gameExecutable: './build/platformer_prototype',
    testOutputDir: './tests/image-regression/output',
    referenceDir: './tests/image-regression/reference',
    diffDir: './tests/image-regression/diff',
    screenshotDelay: 2000, // ms to wait before taking screenshot
    tolerance: 0.1, // pixelmatch threshold
    maxAllowedDiffPercentage: 0.05 // 5% pixel difference allowed
};

class ImageRegressionTester {
    constructor() {
        this.testResults = [];
    }

    async setup() {
        // Create necessary directories
        await this.ensureDir(CONFIG.testOutputDir);
        await this.ensureDir(CONFIG.referenceDir);
        await this.ensureDir(CONFIG.diffDir);
    }

    async ensureDir(dirPath) {
        try {
            await fs.access(dirPath);
        } catch {
            await fs.mkdir(dirPath, { recursive: true });
        }
    }

    async captureScreenshot(testName, gameArgs = []) {
        const outputPath = path.join(CONFIG.testOutputDir, `${testName}.png`);
        
        console.log(`Capturing screenshot for test: ${testName}`);
        
        // Build the command to run the game with screenshot capture
        const args = [
            '--screenshot', outputPath.replace('.png', ''), // Remove .png as the game adds it
            '--screenshot-delay', '1000',
            '--testMode',
            '--width', '800',
            '--height', '600',
            ...gameArgs
        ];
        
        const command = `xvfb-run -a -s "-screen 0 800x600x24" ${CONFIG.gameExecutable} ${args.join(' ')}`;
        
        try {
            console.log(`Running: ${command}`);
            const { stdout, stderr } = await execAsync(command);
            
            if (stderr && !stderr.includes('[info]') && !stderr.includes('[debug]')) {
                console.warn(`Game stderr: ${stderr}`);
            }
            
            // Check if screenshot file was created
            try {
                await fs.access(outputPath);
                console.log(`Screenshot saved: ${outputPath}`);
                return outputPath;
            } catch (error) {
                throw new Error(`Screenshot file not created: ${outputPath}`);
            }
            
        } catch (error) {
            console.error(`Failed to capture screenshot for ${testName}:`, error);
            throw error;
        }
    }

    async createTestImage(outputPath, testName) {
        // Create a test image for demonstration
        // In a real implementation, this would be replaced with actual screenshot capture
        const image = new Jimp({ width: 800, height: 600, color: 0x000000ff }); // Black background
        
        // Add some test content based on test name
        const font = await Jimp.loadFont(Jimp.FONT_SANS_32_WHITE);
        image.print(font, 10, 10, `Test: ${testName}`);
        
        // Add some colored rectangles to simulate game content
        if (testName.includes('character')) {
            image.scan(100, 400, 50, 100, function (x, y, idx) {
                this.bitmap.data[idx + 0] = 255; // Red
                this.bitmap.data[idx + 1] = 0;   // Green
                this.bitmap.data[idx + 2] = 0;   // Blue
                this.bitmap.data[idx + 3] = 255; // Alpha
            });
        }
        
        if (testName.includes('level')) {
            // Add green platforms
            image.scan(0, 550, 800, 50, function (x, y, idx) {
                this.bitmap.data[idx + 0] = 0;   // Red
                this.bitmap.data[idx + 1] = 255; // Green
                this.bitmap.data[idx + 2] = 0;   // Blue
                this.bitmap.data[idx + 3] = 255; // Alpha
            });
        }
        
        await image.write(outputPath);
    }

    async compareImages(testName) {
        const outputPath = path.join(CONFIG.testOutputDir, `${testName}.png`);
        const referencePath = path.join(CONFIG.referenceDir, `${testName}.png`);
        const diffPath = path.join(CONFIG.diffDir, `${testName}-diff.png`);

        try {
            await fs.access(referencePath);
        } catch {
            console.log(`No reference image found for ${testName}, copying current output as reference`);
            await fs.copyFile(outputPath, referencePath);
            return { passed: true, isNewReference: true };
        }

        const [outputImg, referenceImg] = await Promise.all([
            Jimp.read(outputPath),
            Jimp.read(referencePath)
        ]);

        const { width, height } = outputImg.bitmap;
        
        if (width !== referenceImg.bitmap.width || height !== referenceImg.bitmap.height) {
            console.error(`Image dimensions differ for ${testName}`);
            return { passed: false, error: 'Dimension mismatch' };
        }

        const diff = new Jimp({ width, height });
        const pixelsDifferent = pixelmatch(
            outputImg.bitmap.data,
            referenceImg.bitmap.data,
            diff.bitmap.data,
            width,
            height,
            { threshold: CONFIG.tolerance }
        );

        const totalPixels = width * height;
        const diffPercentage = (pixelsDifferent / totalPixels) * 100;

        await diff.write(diffPath);

        const passed = diffPercentage <= CONFIG.maxAllowedDiffPercentage;

        return {
            passed,
            pixelsDifferent,
            totalPixels,
            diffPercentage: parseFloat(diffPercentage.toFixed(4)),
            diffPath
        };
    }

    async runTest(testName, gameArgs = []) {
        console.log(`\n=== Running test: ${testName} ===`);
        
        try {
            // Capture screenshot
            await this.captureScreenshot(testName, gameArgs);
            
            // Compare with reference
            const result = await this.compareImages(testName);
            
            this.testResults.push({
                testName,
                ...result,
                timestamp: new Date().toISOString()
            });

            if (result.passed) {
                console.log(`✅ Test ${testName} passed`);
                if (result.isNewReference) {
                    console.log(`   (Created new reference image)`);
                } else {
                    console.log(`   Diff: ${result.diffPercentage}% (${result.pixelsDifferent}/${result.totalPixels} pixels)`);
                }
            } else {
                console.log(`❌ Test ${testName} failed`);
                console.log(`   Diff: ${result.diffPercentage}% (${result.pixelsDifferent}/${result.totalPixels} pixels)`);
                console.log(`   Diff image: ${result.diffPath}`);
            }

        } catch (error) {
            console.error(`❌ Test ${testName} failed with error:`, error.message);
            this.testResults.push({
                testName,
                passed: false,
                error: error.message,
                timestamp: new Date().toISOString()
            });
        }
    }

    async runAllTests() {
        console.log('🎮 Starting image regression tests for platformer_prototype');
        
        await this.setup();

        // Define test scenarios
        const tests = [
            { name: 'main-menu', args: [] },
            { name: 'level-start', args: ['--levelName', 'test_level'] },
            { name: 'character-spawn', args: ['--levelName', 'test_level'] },
            { name: 'developer-mode', args: ['--developerMode'] },
        ];

        // Run each test
        for (const test of tests) {
            await this.runTest(test.name, test.args);
        }

        // Generate report
        await this.generateReport();
    }

    async generateReport() {
        const reportPath = path.join(CONFIG.testOutputDir, 'test-report.json');
        const htmlReportPath = path.join(CONFIG.testOutputDir, 'test-report.html');

        const report = {
            timestamp: new Date().toISOString(),
            summary: {
                total: this.testResults.length,
                passed: this.testResults.filter(r => r.passed).length,
                failed: this.testResults.filter(r => !r.passed).length
            },
            tests: this.testResults
        };

        await fs.writeFile(reportPath, JSON.stringify(report, null, 2));

        // Generate HTML report
        const htmlReport = this.generateHtmlReport(report);
        await fs.writeFile(htmlReportPath, htmlReport);

        console.log(`\n📊 Test Results Summary:`);
        console.log(`   Total: ${report.summary.total}`);
        console.log(`   Passed: ${report.summary.passed}`);
        console.log(`   Failed: ${report.summary.failed}`);
        console.log(`\n📄 Reports generated:`);
        console.log(`   JSON: ${reportPath}`);
        console.log(`   HTML: ${htmlReportPath}`);

        // Exit with error if any tests failed
        if (report.summary.failed > 0) {
            process.exit(1);
        }
    }

    generateHtmlReport(report) {
        return `
<!DOCTYPE html>
<html>
<head>
    <title>Image Regression Test Report</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .summary { background: #f5f5f5; padding: 15px; border-radius: 5px; margin-bottom: 20px; }
        .test { border: 1px solid #ddd; margin: 10px 0; padding: 15px; border-radius: 5px; }
        .passed { border-left: 5px solid #4CAF50; }
        .failed { border-left: 5px solid #f44336; }
        .image-comparison { display: flex; gap: 10px; margin: 10px 0; }
        .image-container { text-align: center; }
        .image-container img { max-width: 300px; border: 1px solid #ddd; }
    </style>
</head>
<body>
    <h1>Image Regression Test Report</h1>
    <div class="summary">
        <h2>Summary</h2>
        <p>Generated: ${report.timestamp}</p>
        <p>Total Tests: ${report.summary.total}</p>
        <p>Passed: ${report.summary.passed}</p>
        <p>Failed: ${report.summary.failed}</p>
    </div>
    
    <h2>Test Results</h2>
    ${report.tests.map(test => `
        <div class="test ${test.passed ? 'passed' : 'failed'}">
            <h3>${test.testName} ${test.passed ? '✅' : '❌'}</h3>
            ${test.diffPercentage !== undefined ? 
                `<p>Pixel difference: ${test.diffPercentage}% (${test.pixelsDifferent}/${test.totalPixels})</p>` : ''}
            ${test.error ? `<p>Error: ${test.error}</p>` : ''}
        </div>
    `).join('')}
</body>
</html>`;
    }
}

// CLI usage
if (require.main === module) {
    const tester = new ImageRegressionTester();
    
    const args = process.argv.slice(2);
    if (args.includes('--update-references')) {
        console.log('🔄 Updating reference images...');
        // Remove existing reference images
        // This would be implemented to regenerate all reference images
    }
    
    tester.runAllTests().catch(error => {
        console.error('Test suite failed:', error);
        process.exit(1);
    });
}

module.exports = ImageRegressionTester;