# Contributing to EdgeVision Pro / MicroVision AI

First off, thank you for considering a contribution to our project! It's people like you that make these projects such great tools.

## 📋 Code of Conduct

This project and everyone participating in it is governed by our [Code of Conduct](CODE_OF_CONDUCT.md). By participating, you are expected to uphold this code.

## ❓ How Can I Contribute?

### 🐛 Reporting Bugs

Before creating bug reports, please check the issue list as you might find out that you don't need to create one. When you are creating a bug report, please include as many details as possible:

* **Use a clear and descriptive title**
* **Describe the exact steps which reproduce the problem**
* **Provide specific examples to demonstrate the steps**
* **Describe the behavior you observed after following the steps**
* **Explain which behavior you expected to see instead and why**
* **Include hardware details** (ESP32 variant, camera model, OLED display, etc.)
* **Include Arduino IDE version and board package version**
* **Include screenshots or screen recordings if applicable**

**Bug Report Template:**
```markdown
## Title: Brief description of the bug

**Hardware Configuration:**
- ESP32 Variant: [e.g., DevKit V1, AI_THINKER]
- Camera Model: [if applicable]
- Display: [if applicable]
- Arduino IDE Version: [e.g., 2.3.2]
- Board Package Version: [e.g., 3.0.0]

**Steps to Reproduce:**
1. [First Step]
2. [Second Step]
3. [etc...]

**Expected Behavior:**
[What should happen]

**Actual Behavior:**
[What actually happens]

**Serial Output:**
[Paste relevant serial monitor output]
```

### ✨ Suggesting Enhancements

Enhancement suggestions are tracked as GitHub issues. When creating an enhancement suggestion, please include:

* **Use a clear and descriptive title**
* **Provide a step-by-step description of the suggested enhancement**
* **Provide specific examples to demonstrate the steps**
* **Describe the current behavior** and **expected behavior**
* **Explain why this enhancement would be useful**

**Feature Request Template:**
```markdown
## Title: Brief description of the feature

**Motivation:**
[Why is this feature needed? What problem does it solve?]

**Proposed Solution:**
[How should this feature work?]

**Alternatives Considered:**
[Any alternative approaches?]

**Additional Context:**
[Diagrams, mockups, examples, etc.]
```

### 📝 Code Contributions

#### Before You Start
1. **Fork the repository** on GitHub
2. **Clone your fork** locally:
   ```bash
   git clone https://github.com/YOUR-USERNAME/project-name.git
   cd project-name
   ```
3. **Create a new branch** for your feature:
   ```bash
   git checkout -b feature/your-feature-name
   # or for bug fixes:
   git checkout -b fix/bug-description
   ```

#### Development Guidelines

**Code Style:**
- Use **clear variable names** that describe their purpose
- Follow **Arduino naming conventions**
  - Functions: `camelCase` (e.g., `captureImage()`)
  - Variables: `snake_case` (e.g., `image_buffer`)
  - Constants: `UPPER_CASE` (e.g., `MAX_BUFFER_SIZE`)
- **Maximum line length: 100 characters**
- Use **2-space indentation** (not tabs)
- Add **comments for complex logic**

**Code Quality:**
```cpp
// ❌ BAD - No comments, unclear variable names
void process() {
  int a = 320;
  int b = 240;
  uint8_t *buf = (uint8_t*)malloc(a * b * 3);
  // ... code ...
  free(buf);
}

// ✅ GOOD - Clear structure, meaningful names, comments
void capture_and_process_image() {
  // Define image dimensions
  const uint16_t img_width = 320;
  const uint16_t img_height = 240;
  const uint16_t bytes_per_pixel = 3; // RGB888 format
  
  // Allocate buffer for raw image data
  uint32_t buffer_size = img_width * img_height * bytes_per_pixel;
  uint8_t *image_buffer = (uint8_t*)malloc(buffer_size);
  
  if (image_buffer == nullptr) {
    ei_printf("ERROR: Failed to allocate image buffer\n");
    return false;
  }
  
  // Process image...
  
  // Always free allocated memory
  free(image_buffer);
}
```

**Header Documentation:**
Every file should start with a header:
```cpp
/*
 * @file image_processor.cpp
 * @brief Image processing functions for camera capture and ML preprocessing
 * 
 * @author Pavan Kumar Ginkala
 * @version 1.0
 * @date 2025-01-15
 * 
 * @details
 * This module provides functions for:
 * - Capturing images from ESP32 camera module
 * - Converting JPEG to RGB888 format
 * - Resizing images for neural network input
 * 
 * @dependencies
 * - esp_camera.h
 * - image.hpp (Edge Impulse SDK)
 * 
 * @see https://github.com/espressif/arduino-esp32/tree/master/libraries/ESP32/examples/Camera
 */
```

**Function Documentation:**
```cpp
/**
 * @brief Captures an image from the ESP32 camera
 * 
 * @param width      Target image width in pixels
 * @param height     Target image height in pixels
 * @param out_buf    Output buffer for RGB888 image data
 * 
 * @return bool      true if capture successful, false otherwise
 * 
 * @pre Camera must be initialized via ei_camera_init()
 * @post Image data is written to out_buf
 * 
 * @note
 * - Allocate out_buf with size: width * height * 3 bytes
 * - Function may take up to 100ms on first call
 * - Buffer must be freed by caller after use
 * 
 * @example
 * uint8_t *buffer = (uint8_t*)malloc(320 * 240 * 3);
 * if (capture_image(320, 240, buffer)) {
 *     process_image(buffer);
 * }
 * free(buffer);
 */
bool capture_image(uint16_t width, uint16_t height, uint8_t *out_buf);
```

**Testing:**
Before submitting, test your code:
- Compile without errors or warnings
- Test on actual hardware
- Test edge cases (low memory, slow network, etc.)
- Verify no memory leaks with malloc/free
- Check serial output for errors

```cpp
// Example test case structure
void test_image_capture() {
  Serial.println("TEST: Image Capture");
  
  // Test 1: Normal capture
  uint8_t *buf = (uint8_t*)malloc(320 * 240 * 3);
  assert(capture_image(320, 240, buf) == true);
  Serial.println("✓ Image capture successful");
  free(buf);
  
  // Test 2: Null buffer handling
  assert(capture_image(320, 240, nullptr) == false);
  Serial.println("✓ Null buffer properly rejected");
  
  // Test 3: Invalid dimensions
  assert(capture_image(0, 0, buf) == false);
  Serial.println("✓ Invalid dimensions rejected");
}
```

#### Commit Messages

Write clear, meaningful commit messages:

```
# Format: <type>(<scope>): <subject>
# 
# <body>
#
# <footer>

# Example:
feat(camera): Add support for multiple camera models

- Implement camera model abstraction layer
- Add AI_THINKER and ESP_EYE support
- Maintain backward compatibility with existing code

Closes #123, #456
```

**Types:**
- `feat:` A new feature
- `fix:` A bug fix
- `docs:` Documentation only changes
- `style:` Changes that don't affect code meaning
- `refactor:` Code change that neither fixes a bug nor adds a feature
- `perf:` Code change that improves performance
- `test:` Adding or updating tests
- `chore:` Changes to build process, dependencies, etc.

#### Branching Strategy

```
main (stable releases)
├── develop (integration branch)
│   ├── feature/camera-upgrade
│   ├── feature/ai-optimization
│   └── fix/memory-leak
└── release/v1.1 (release candidates)
```

**Branch naming:**
- Feature: `feature/short-description`
- Bug fix: `fix/short-description`
- Documentation: `docs/short-description`
- Release: `release/vX.Y.Z`

### 📚 Documentation Improvements

Documentation improvements are always welcome! You can:

* Add examples and tutorials
* Improve existing documentation
* Add diagrams and flowcharts
* Translate documentation
* Fix typos and grammatical errors
* Add troubleshooting guides

**Documentation Style Guide:**
- Use **clear, simple language**
- Include **code examples** where relevant
- Add **warnings** for common mistakes (⚠️)
- Include **tips** for best practices (💡)
- Provide **before/after examples**
- Keep **sections well-organized** with headers

### 🔍 Pull Request Process

1. **Update documentation** with any new features or changes
2. **Test thoroughly** on actual hardware
3. **Update the CHANGELOG.md** with your changes
4. **Create a Pull Request** with a clear title and description

**Pull Request Template:**
```markdown
## Description
[Describe the changes you've made]

## Related Issue
Closes #[issue number]

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Documentation update
- [ ] Performance improvement
- [ ] Code refactor

## Testing
- [ ] Code compiles without warnings
- [ ] Tested on ESP32 hardware
- [ ] No memory leaks
- [ ] Serial output is clean

## Hardware Tested On
- [ ] ESP32 DevKit V1
- [ ] Camera: [model]
- [ ] Display: [model]

## Screenshots/Logs
[If applicable, add screenshots or serial output]

## Checklist
- [ ] Code follows style guidelines
- [ ] Comments added for complex logic
- [ ] Documentation updated
- [ ] CHANGELOG updated
- [ ] No breaking changes
```

## 💰 Monetary Contributions

If you would like to contribute financially to support development:

- GitHub Sponsors: [Coming Soon](#)
- Patreon: [Coming Soon](#)
- Buy Me a Coffee: [Coming Soon](#)

## 📞 Getting Help

* **Stuck?** Ask in [GitHub Discussions](https://github.com/pavankumar-21/EdgeVision-Pro/discussions)
* **Email:** ginkalapavankumar@gmail.com
* **Issues:** Check [GitHub Issues](https://github.com/pavankumar-21/EdgeVision-Pro/issues)

## 🎓 Development Resources

### Learning Resources
- [Arduino Language Reference](https://www.arduino.cc/reference/en/)
- [ESP32 Arduino Core GitHub](https://github.com/espressif/arduino-esp32)
- [Edge Impulse Documentation](https://docs.edgeimpulse.com/)
- [OpenAI API Documentation](https://platform.openai.com/docs)

### Development Tools
- **Arduino IDE:** https://www.arduino.cc/en/software
- **PlatformIO:** https://platformio.org/ (alternative to Arduino IDE)
- **Visual Studio Code:** https://code.visualstudio.com/
- **Serial Monitor:** Built into Arduino IDE

### Useful Commands
```bash
# Verify code compiles (Arduino CLI)
arduino-cli compile --fqbn esp32:esp32:esp32

# Install library dependencies
arduino-cli lib install ArduinoJson

# Monitor serial output
arduino-cli monitor -p /dev/ttyUSB0 -c baudrate=115200
```

## 🏆 Recognition

Contributors are recognized in:
- [CONTRIBUTORS.md](CONTRIBUTORS.md)
- GitHub contributors page
- Project release notes
- README acknowledgments section

## ⚖️ Legal

By contributing to this project, you agree that:
- You own the copyright to your contribution
- You are licensing your contribution under the project's MIT License
- You have the right to grant these rights

---

**Thank you for contributing to making EdgeVision Pro / MicroVision AI better! 🚀**

Questions? Feel free to reach out!
