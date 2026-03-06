/*
 * @file main.cpp
 * @brief EdgeVision Pro - Real-time object detection on ESP32
 * 
 * Main firmware for AI-powered object detection using Edge Impulse
 * and ESP32 with OV3660 camera and SSD1306 OLED display
 * 
 * @author Pavan Kumar Ginkala
 * @version 1.0.0
 * @date 2025-01-15
 * 
 * Hardware: ESP32 DevKit V1 (30-pin), AI_THINKER camera, SSD1306 OLED
 * Dependencies: Edge Impulse SDK, Adafruit libraries, ESP32 core
 */

// These sketches are tested with 2.0.4 ESP32 Arduino Core
// https://github.com/espressif/arduino-esp32/releases/tag/2.0.4

/* Includes ---------------------------------------------------------------- */
#include <SaiRam-project-1_inferencing.h>
#include "edge-impulse-sdk/dsp/image/image.hpp"
#include "esp_camera.h"

// --- OLED Display includes ---
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED Display Parameters
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< 0x3C or 0x3D

// Custom I2C pins for OLED
#define OLED_SDA_PIN 13 
#define OLED_SCL_PIN 15

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Select camera model
#define CAMERA_MODEL_AI_THINKER // Has PSRAM

// Camera pin configuration for AI_THINKER
#if defined(CAMERA_MODEL_AI_THINKER)
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM       5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22
#else
#error "Camera model not selected"
#endif

/* Constant defines -------------------------------------------------------- */
#define EI_CAMERA_RAW_FRAME_BUFFER_COLS           320
#define EI_CAMERA_RAW_FRAME_BUFFER_ROWS           240
#define EI_CAMERA_FRAME_BYTE_SIZE                 3

/* Private variables ------------------------------------------------------- */
static bool debug_nn = false; 
static bool is_initialised = false;
uint8_t *snapshot_buf; 

static camera_config_t camera_config = {
    .pin_pwdn = PWDN_GPIO_NUM,
    .pin_reset = RESET_GPIO_NUM,
    .pin_xclk = XCLK_GPIO_NUM,
    .pin_sscb_sda = SIOD_GPIO_NUM,
    .pin_sscb_scl = SIOC_GPIO_NUM,

    .pin_d7 = Y9_GPIO_NUM,
    .pin_d6 = Y8_GPIO_NUM,
    .pin_d5 = Y7_GPIO_NUM,
    .pin_d4 = Y6_GPIO_NUM,
    .pin_d3 = Y5_GPIO_NUM,
    .pin_d2 = Y4_GPIO_NUM,
    .pin_d1 = Y3_GPIO_NUM,
    .pin_d0 = Y2_GPIO_NUM,
    .pin_vsync = VSYNC_GPIO_NUM,
    .pin_href = HREF_GPIO_NUM,
    .pin_pclk = PCLK_GPIO_NUM,

    .xclk_freq_hz = 20000000,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,

    .pixel_format = PIXFORMAT_JPEG, 
    .frame_size = FRAMESIZE_QVGA,    

    .jpeg_quality = 12, 
    .fb_count = 1,         
    .fb_location = CAMERA_FB_IN_PSRAM,
    .grab_mode = CAMERA_GRAB_WHEN_EMPTY,
};

/* Function definitions ------------------------------------------------------- */
bool ei_camera_init(void);
void ei_camera_deinit(void);
bool ei_camera_capture(uint32_t img_width, uint32_t img_height, uint8_t *out_buf);
static int ei_camera_get_data(size_t offset, size_t length, float *out_ptr);

/**
 * @brief Arduino setup function - Initialize system
 */
void setup()
{
    Serial.begin(115200);
    while (!Serial);
    Serial.println("Edge Impulse Inferencing Demo");

    // Initialize I2C with custom pins
    Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN); 

    // Initialize OLED display
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
    }
    
    // Clear and initialize display
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("EI-Camera Init...");
    display.display();

    // Initialize camera
    if (ei_camera_init() == false) {
        ei_printf("Failed to initialize Camera!\r\n");
        display.setCursor(0, 10);
        display.println("Camera FAILED!");
        display.display();
    }
    else {
        ei_printf("Camera initialized\r\n");
        display.setCursor(0, 10);
        display.println("Camera OK!");
        display.display();
    }

    ei_printf("\nStarting continuous inference in 2 seconds...\n");
    ei_sleep(2000);
}

/**
 * @brief Main loop - Continuous inference
 */
void loop()
{
    if (ei_sleep(5) != EI_IMPULSE_OK) {
        return;
    }

    snapshot_buf = (uint8_t*)malloc(EI_CAMERA_RAW_FRAME_BUFFER_COLS * EI_CAMERA_RAW_FRAME_BUFFER_ROWS * EI_CAMERA_FRAME_BYTE_SIZE);

    if(snapshot_buf == nullptr) {
        ei_printf("ERR: Failed to allocate snapshot buffer!\n");
        return;
    }

    ei::signal_t signal;
    signal.total_length = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT;
    signal.get_data = &ei_camera_get_data;

    if (ei_camera_capture((size_t)EI_CLASSIFIER_INPUT_WIDTH, (size_t)EI_CLASSIFIER_INPUT_HEIGHT, snapshot_buf) == false) {
        ei_printf("Failed to capture image\r\n");
        free(snapshot_buf);
        return;
    }

    ei_impulse_result_t result = { 0 };

    EI_IMPULSE_ERROR err = run_classifier(&signal, &result, debug_nn);
    if (err != EI_IMPULSE_OK) {
        ei_printf("ERR: Failed to run classifier (%d)\n", err);
        return;
    }

    ei_printf("Predictions (DSP: %d ms., Classification: %d ms., Anomaly: %d ms.): \n",
                 result.timing.dsp, result.timing.classification, result.timing.anomaly);

    // Display results on OLED
    display.clearDisplay(); 
    display.setTextSize(2); 
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0); 
    
    float max_score = 0;
    int max_index = -1;
    
#if EI_CLASSIFIER_OBJECT_DETECTION == 1
    bool object_found = false;
    for (uint32_t i = 0; i < result.bounding_boxes_count; i++) {
        ei_impulse_result_bounding_box_t bb = result.bounding_boxes[i];
        if (bb.value > 0) {
            display.println(bb.label);
            display.setTextSize(1);
            display.print("Conf: ");
            display.println(bb.value, 2);
            display.print("Box: (");
            display.print(bb.x);
            display.print(",");
            display.print(bb.y);
            display.print(") ");
            display.print(bb.width);
            display.print("x");
            display.println(bb.height);
            object_found = true;
            break; 
        }
    }
    if (!object_found) {
        display.println("No Object");
    }
#else
    for (uint16_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
        if (result.classification[i].value > max_score) {
            max_score = result.classification[i].value;
            max_index = i;
        }
    }
    
    if (max_index != -1) {
        display.println(ei_classifier_inferencing_categories[max_index]);
        display.setTextSize(1);
        display.print("Conf: ");
        display.println(max_score, 2); 
    } else {
        display.println("No Result");
    }
#endif
    
#if EI_CLASSIFIER_HAS_ANOMALY
    display.setCursor(0, SCREEN_HEIGHT - 10); 
    display.setTextSize(1);
    display.print("Anomaly: ");
    display.println(result.anomaly, 3);
#endif

    display.display(); 

#if EI_CLASSIFIER_OBJECT_DETECTION == 1
    ei_printf("Object detection bounding boxes:\r\n");
    for (uint32_t i = 0; i < result.bounding_boxes_count; i++) {
        ei_impulse_result_bounding_box_t bb = result.bounding_boxes[i];
        if (bb.value == 0) {
            continue;
        }
        ei_printf("  %s (%f) [ x: %u, y: %u, width: %u, height: %u ]\r\n",
                bb.label,
                bb.value,
                bb.x,
                bb.y,
                bb.width,
                bb.height);
    }
#else
    ei_printf("Predictions:\r\n");
    for (uint16_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
        ei_printf("  %s: ", ei_classifier_inferencing_categories[i]);
        ei_printf("%.5f\r\n", result.classification[i].value);
    }
#endif

#if EI_CLASSIFIER_HAS_ANOMALY
    ei_printf("Anomaly prediction: %.3f\r\n", result.anomaly);
#endif

#if EI_CLASSIFIER_HAS_VISUAL_ANOMALY
    ei_printf("Visual anomalies:\r\n");
    for (uint32_t i = 0; i < result.visual_ad_count; i++) {
        ei_impulse_result_bounding_box_t bb = result.visual_ad_grid_cells[i];
        if (bb.value == 0) {
            continue;
        }
        ei_printf("  %s (%f) [ x: %u, y: %u, width: %u, height: %u ]\r\n",
                bb.label,
                bb.value,
                bb.x,
                bb.y,
                bb.width,
                bb.height);
    }
#endif

    free(snapshot_buf);
}

/**
 * @brief Initialize camera module
 * @return true if successful, false otherwise
 */
bool ei_camera_init(void) {
    if (is_initialised) return true;
    
    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK) {
      Serial.printf("Camera init failed with error 0x%x\n", err);
      return false;
    }
    
    sensor_t * s = esp_camera_sensor_get();
    if (s->id.PID == OV3660_PID) {
      s->set_vflip(s, 1); 
      s->set_brightness(s, 1); 
      s->set_saturation(s, 0); 
    }
    
    is_initialised = true;
    return true;
}

/**
 * @brief Deinitialize camera
 */
void ei_camera_deinit(void) {
    esp_err_t err = esp_camera_deinit();
    if (err != ESP_OK) {
        ei_printf("Camera deinit failed\n");
        return;
    }
    is_initialised = false;
}

/**
 * @brief Capture image from camera
 * @param img_width Target width
 * @param img_height Target height
 * @param out_buf Output buffer for image data
 * @return true if successful, false otherwise
 */
bool ei_camera_capture(uint32_t img_width, uint32_t img_height, uint8_t *out_buf) {
    bool do_resize = false;
    if (!is_initialised) {
        ei_printf("ERR: Camera is not initialized\r\n");
        return false;
    }
    
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        ei_printf("Camera capture failed\n");
        return false;
    }
    
   bool converted = fmt2rgb888(fb->buf, fb->len, PIXFORMAT_JPEG, snapshot_buf);
   esp_camera_fb_return(fb);
   if(!converted){
        ei_printf("Conversion failed\n");
        return false;
   }
    
    if ((img_width != EI_CAMERA_RAW_FRAME_BUFFER_COLS)
        || (img_height != EI_CAMERA_RAW_FRAME_BUFFER_ROWS)) {
        do_resize = true;
    }
    
    if (do_resize) {
        ei::image::processing::crop_and_interpolate_rgb888(
        out_buf,
        EI_CAMERA_RAW_FRAME_BUFFER_COLS,
        EI_CAMERA_RAW_FRAME_BUFFER_ROWS,
        out_buf,
        img_width,
        img_height);
    }
    
    return true;
}

/**
 * @brief Get image data for ML inference
 * @param offset Pixel offset
 * @param length Number of pixels
 * @param out_ptr Output data pointer
 * @return 0 on success
 */
static int ei_camera_get_data(size_t offset, size_t length, float *out_ptr)
{
    size_t pixel_ix = offset * 3;
    size_t pixels_left = length;
    size_t out_ptr_ix = 0;
    
    while (pixels_left != 0) {
        out_ptr[out_ptr_ix] = (snapshot_buf[pixel_ix + 2] << 16) + (snapshot_buf[pixel_ix + 1] << 8) + snapshot_buf[pixel_ix];
        out_ptr_ix++;
        pixel_ix+=3;
        pixels_left--;
    }
    
    return 0;
}

#if !defined(EI_CLASSIFIER_SENSOR) || EI_CLASSIFIER_SENSOR != EI_CLASSIFIER_SENSOR_CAMERA
#error "Invalid model for current sensor"
#endif
