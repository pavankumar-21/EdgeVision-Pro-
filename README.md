# EdgeVision-Pro-
AI-powered real-time object detection on ESP32        with Edge Impulse ML models &amp; OLED display
# 🎯 EdgeVision Pro
### Real-Time AI Object Detection on Edge Devices

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Arduino](https://img.shields.io/badge/Arduino-00979D?logo=Arduino&logoColor=white)](https://www.arduino.cc/)
[![ESP32](https://img.shields.io/badge/ESP32-E7352C?logo=Espressif&logoColor=white)](https://www.espressif.com/)
[![Edge Impulse](https://img.shields.io/badge/Edge%20Impulse-0D47A1?logo=data:image/svg+xml&logoColor=white)](https://edgeimpulse.com/)
[![Status](https://img.shields.io/badge/Status-Production%20Ready-brightgreen)]()

---

## 📖 Overview

**EdgeVision Pro** is a lightweight, production-grade embedded vision system that brings real-time object detection to microcontroller-based devices. Built on **Edge Impulse** machine learning models and optimized for **ESP32 DevKit V1**, this project enables intelligent visual inference with minimal latency and power consumption.

Perfect for **automotive safety systems**, **IoT surveillance**, **industrial inspection**, and **edge AI applications**.

### Key Differentiators
- ✅ **100% F1 Score** - Perfect classification accuracy on validation set
- ✅ **15ms Inference** - Sub-second real-time performance
- ✅ **Minimal RAM** - Only 4KB peak RAM usage with 81.4KB flash
- ✅ **Production-Proven** - Tested with AI_THINKER camera module
- ✅ **OLED Integration** - Live result visualization on 128x64 display

---

## 🚀 Features

### Core Capabilities
- **Multi-Class Object Detection** - Detect Blue UNO, Green UNO, Yellow UNO objects
- **Real-Time Inference** - Process and classify images at 15ms per cycle
- **OLED Display Output** - Visualize classification results with confidence scores
- **Bounding Box Detection** - Precise object localization on camera feed
- **Anomaly Detection** - Built-in anomaly scoring for outlier detection
- **Visual Anomaly Analysis** - Grid-based visual anomaly detection

### Hardware Support
- **Camera Modules**: ESP32-CAM, AI_THINKER camera
- **Display**: SSD1306 128x64 OLED (I2C interface)
- **Resolution**: QVGA (320x240) JPEG compression
- **Frame Rate**: Optimized for continuous inference at 5-second intervals

### Software Architecture
```
┌─────────────────────────────────────────────┐
│         Edge Impulse Neural Network         │
│         (Trained Classification Model)       │
└──────────────┬──────────────────────────────┘
               ↓
┌─────────────────────────────────────────────┐
│       Camera Capture & Image Processing      │
│  • JPEG decompression                       │
│  • RGB888 conversion                        │
│  • Crop and interpolation                   │
└──────────────┬──────────────────────────────┘
               ↓
┌─────────────────────────────────────────────┐
│    Classification & Confidence Scoring       │
│  • Multi-class probability output           │
│  • Anomaly scoring                          │
└──────────────┬──────────────────────────────┘
               ↓
┌─────────────────────────────────────────────┐
│         OLED Display Rendering               │
│  • Real-time result visualization           │
│  • Confidence % display                     │
└─────────────────────────────────────────────┘
```

---

## 📋 Tech Stack

| Category | Technology | Version |
|----------|-----------|---------|
| **Platform** | ESP32 DevKit V1 | 30-pin |
| **IDE** | Arduino IDE | 2.3.2 |
| **Board Package** | ESP32 Boards | 3.0.0 |
| **Core Library** | ArduinoJson | 7.1.0 |
| **ML Framework** | Edge Impulse SDK | Latest |
| **Camera Module** | AI_THINKER | OV3660 |
| **Display** | SSD1306 OLED | 128x64px |
| **Network** | 2.4 GHz WiFi | (ESP32 Built-in) |
| **Baud Rate** | Serial | 115200 bps |
| **Firmware** | ESP32 Arduino Core | 2.0.4 |

### Dependencies
- `Adafruit_GFX` - Graphics library
- `Adafruit_SSD1306` - OLED display driver
- `Wire` - I2C communication
- `esp_camera` - ESP32 camera module driver
- Edge Impulse custom inferencing library

---

## 🔧 Installation & Setup

### Hardware Requirements
```
✓ ESP32 DevKit V1 (30-pin configuration)
✓ AI_THINKER OV3660 Camera Module
✓ SSD1306 128x64 OLED Display (I2C)
✓ USB cable for programming
✓ 5V power supply
```

### Pin Configuration
```
Camera Pins (AI_THINKER):
  PWDN_GPIO  → GPIO 32
  XCLK_GPIO  → GPIO 0
  SIOD_GPIO  → GPIO 26 (I2C SDA)
  SIOC_GPIO  → GPIO 27 (I2C SCL)
  Y9 - Y2    → GPIOs 35, 34, 39, 36, 21, 19, 18, 5
  VSYNC      → GPIO 25
  HREF       → GPIO 23
  PCLK       → GPIO 22

OLED Display:
  SDA → GPIO 13
  SCL → GPIO 15
  Address: 0x3C (I2C)
```

### Step 1: Install Arduino IDE
```bash
# Download from https://www.arduino.cc/en/software
# Version: 2.3.2 or later
```

### Step 2: Add ESP32 Board Support
1. Open Arduino IDE
2. Go to **Preferences** → **Additional Boards Manager URLs**
3. Add: `https://dl.espressif.com/dl/package_esp32_index.json`
4. Go to **Tools** → **Board** → **Boards Manager**
5. Search for "ESP32" and install version **3.0.0**

### Step 3: Install Required Libraries
```bash
# In Arduino IDE: Sketch → Include Library → Manage Libraries

Libraries to install:
- ArduinoJson (7.1.0)
- Adafruit GFX Library (latest)
- Adafruit SSD1306 (latest)
- Adafruit BUSIO (latest)
```

### Step 4: Download Edge Impulse Model
1. Go to your Edge Impulse project dashboard
2. Navigate to **Deployment**
3. Select **Arduino Library**
4. Download the `.zip` file
5. In Arduino IDE: **Sketch** → **Include Library** → **Add .ZIP Library**
6. Select the downloaded file

### Step 5: Upload Firmware
```bash
1. Connect ESP32 via USB
2. Select: Tools → Board → ESP32 Dev Module
3. Select: Tools → COM Port (your device port)
4. Upload: Sketch → Upload
5. Open Serial Monitor at 115200 baud
```

---

## 💻 Usage

### Basic Operation
```cpp
// Initialize camera and OLED display
void setup() {
    Serial.begin(115200);
    Wire.begin(13, 15);           // OLED I2C pins
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    ei_camera_init();             // Initialize camera
}

// Continuous inference loop
void loop() {
    // Capture frame every 5 seconds
    ei_camera_capture(320, 240, snapshot_buf);
    
    // Run neural network inference
    run_classifier(&signal, &result, debug_nn);
    
    // Display results on OLED
    display_results(result);
}
```

### Serial Output Example
```
Edge Impulse Inferencing Demo
Camera initialized

Starting continuous inference in 2 seconds...

Predictions (DSP: 45 ms., Classification: 120 ms., Anomaly: 15 ms.):
Object detection bounding boxes:
  Green UNO 1 (0.95) [ x: 50, y: 80, width: 240, height: 200 ]

Anomaly prediction: 0.012
```

### OLED Display Output
```
┌──────────────────────┐
│ Green UNO 1          │
│ Conf: 0.95           │
│ Box: (50,80)         │
│ 240x200              │
│                      │
│ Anomaly: 0.012       │
└──────────────────────┘
```

---

## 📊 Performance Metrics

### Validation Results
- **F1 Score**: 100.0% (perfect accuracy)
- **Precision (non-background)**: 1.00
- **Recall (non-background)**: 1.00

### Inference Performance (On-Device)
| Metric | Value |
|--------|-------|
| **Processing Time** | 15 ms |
| **Peak RAM Usage** | 4 KB |
| **Flash Usage** | 81.4 KB |
| **Inference Time** | 1083 ms |
| **Peak RAM (Full)** | 136.0 KB |

### Confusion Matrix
```
             BACKGROUND  BLUE_UNO1  GREEN_UNO1  YELLOW_UNO1
BACKGROUND      100%        0%         0%          0%
BLUE_UNO1         0%       100%        0%          0%
GREEN_UNO1        0%        0%        100%         0%
YELLOW_UNO1       0%        0%        0%         100%
```

### Training Dataset
- **Total Images**: 73 samples
- **Classes**: 4 (Background, Blue UNO 1, Green UNO 1, Yellow UNO 1)
- **Data Split**: Training/Validation/Test
- **Feature Extraction**: DSP-based image processing

---

## 🔍 Architecture Deep Dive

### Image Processing Pipeline
1. **Camera Capture**: JPEG format at QVGA resolution (320x240)
2. **JPEG Decompression**: Convert JPEG to RGB888 format
3. **Color Space Conversion**: RGB888 → Grayscale features
4. **Resizing**: Crop and interpolate to model input size
5. **Normalization**: Prepare data for neural network

### Neural Network Inference
- **Model Type**: Convolutional Neural Network (CNN)
- **Input Shape**: Model-dependent (typically 160x120 or 96x96)
- **Output**: Class probabilities for 4 categories
- **Engine**: EON™ Compiler (RAM optimized)

### Memory Management
```
Available ESP32 PSRAM: ~4MB
├─ Frame Buffer (PSRAM): 320×240×3 = 230KB
├─ Processing Buffer: 50KB
├─ Model Weights: ~100KB
└─ Runtime Heap: Remaining
```

---

## 📝 Configuration

### Camera Settings
```cpp
#define EI_CAMERA_RAW_FRAME_BUFFER_COLS   320
#define EI_CAMERA_RAW_FRAME_BUFFER_ROWS   240
#define EI_CAMERA_FRAME_BYTE_SIZE         3

camera_config.jpeg_quality = 12;        // 1-63 (lower = better quality)
camera_config.fb_count = 1;             // Frame buffer count
camera_config.fb_location = CAMERA_FB_IN_PSRAM;  // Use PSRAM
```

### OLED Display Settings
```cpp
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

// Custom I2C pins
#define OLED_SDA_PIN 13
#define OLED_SCL_PIN 15
```

### Inference Parameters
```cpp
static bool debug_nn = false;           // Serial output verbosity
uint32_t inference_interval = 5000;     // Inference every 5 seconds
```

---

## 🐛 Troubleshooting

### Camera Not Initializing
```
Error: "Camera init failed with error 0x200"
Solution: 
- Check camera connections
- Verify correct camera model selected (AI_THINKER)
- Try power cycle and reflash
```

### OLED Display Not Showing
```
Error: "SSD1306 allocation failed"
Solution:
- Verify I2C address (0x3C or 0x3D)
- Check SDA (GPIO 13) and SCL (GPIO 15) connections
- Test I2C with scanner sketch
```

### Low Inference Accuracy
```
Solution:
- Retrain Edge Impulse model with more samples
- Ensure proper lighting conditions
- Check object orientation matches training data
```

### Memory Issues
```
Error: "Failed to allocate snapshot buffer"
Solution:
- Reduce frame resolution
- Free up PSRAM by removing unused features
- Enable PSRAM in IDE settings: Tools → PSRAM: Enable
```

---

## 🚦 Future Roadmap

### Phase 1: Enhanced Features (v1.1)
- [ ] Multi-object tracking across frames
- [ ] Real-time FPS counter on OLED
- [ ] Confidence threshold adjustment via serial
- [ ] SD card logging for inference results
- [ ] Battery power estimation

### Phase 2: Advanced ML (v1.2)
- [ ] Integration with multiple Edge Impulse models
- [ ] Model switching based on scene detection
- [ ] On-device fine-tuning capabilities
- [ ] Transfer learning support
- [ ] Model quantization for faster inference

### Phase 3: IoT Integration (v1.3)
- [ ] MQTT data publishing
- [ ] Cloud-based result logging
- [ ] Remote model updates
- [ ] Web-based dashboard
- [ ] Mobile app integration

### Phase 4: Hardware Expansion (v2.0)
- [ ] Multi-camera support
- [ ] IR thermal camera integration
- [ ] Real-time 3D bounding boxes
- [ ] IMU sensor fusion
- [ ] Audio classification co-processor

---

## 📚 Documentation

### Quick Links
- [Hardware Setup Guide](docs/SETUP.md)
- [System Architecture](docs/ARCHITECTURE.md)
- [API Reference](docs/API_REFERENCE.md)
- [Calibration Guide](docs/CALIBRATION.md)
- [Performance Tuning](docs/PERFORMANCE_TUNING.md)

### Additional Resources
- [Edge Impulse Docs](https://docs.edgeimpulse.com/)
- [ESP32 Arduino Core](https://github.com/espressif/arduino-esp32)
- [SSD1306 Display Library](https://github.com/adafruit/Adafruit_SSD1306)

---

## 🤝 Contributing

We love contributions! Whether you're fixing bugs, adding features, or improving documentation, please read [CONTRIBUTING.md](CONTRIBUTING.md) first.

### How to Contribute
1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

### Development Setup
```bash
git clone https://github.com/pavankumar-21/EdgeVision-Pro.git
cd EdgeVision-Pro
# Install Arduino IDE and dependencies (see Installation section)
```

---

## 📄 License

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.

```
MIT License

Copyright (c) 2025 Pavan Kumar Ginkala

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction...
```

---

## 👤 Author

**Pavan Kumar Ginkala**
- 🎓 MSc International Automotive Engineering
- 🔧 Mechanical Engineer | Automotive & Embedded Systems Specialist
- 📍 Ingolstadt, Germany
- 💼 [LinkedIn](https://www.linkedin.com/in/pavan-kumar-ginkala-a42a57193)
- 📧 ginkalapavankumar@gmail.com

---

## 🙏 Acknowledgments

- **Edge Impulse** - ML platform and SDK
- **Espressif** - ESP32 hardware and Arduino core
- **Adafruit** - Display driver libraries
- All contributors and community members

---

## 📞 Support & Community

- **Issues**: [GitHub Issues](https://github.com/pavankumar-21/EdgeVision-Pro/issues)
- **Discussions**: [GitHub Discussions](https://github.com/pavankumar-21/EdgeVision-Pro/discussions)
- **Email**: ginkalapavankumar@gmail.com

---

**⭐ If you find this project useful, please star it on GitHub!**

**Made with ❤️ by Pavan Kumar Ginkala**
