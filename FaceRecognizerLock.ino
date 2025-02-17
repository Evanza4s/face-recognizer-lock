#include "esp_camera.h"
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// WARNING!!! Make sure that you have either selected ESP32 Wrover Module,
// or another board which has PSRAM enabled

// Select camera model
//#define CAMERA_MODEL_WROVER_KIT
//#define CAMERA_MODEL_ESP_EYE
//#define CAMERA_MODEL_M5STACK_PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE
#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"

// WiFi credentials
const char* ssid = "Denis iPhone";
const char* password = "lausanne1265461";

const char* serverUrl = "http://192.168.1.101:5000/recognize"; 

// Ultrasonic sensor
#define TRIG_PIN 12
#define ECHO_PIN 13
#define DISTANCE_THRESHOLD 50

// OLED Display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

boolean matchFace = false;

void startCameraServer();

long getDistance() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH);
    long distance = duration * 0.034 / 2;
    return distance;
}

void updateDisplay(String message) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 20);
    display.println(message);
    display.display();
}

void setup() {
    Serial.begin(115200);
    Serial1.begin(9600, SERIAL_8N1, 2, 3); // TX ke Arduino

    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    // Inisialisasi OLED
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("SSD1306 initialization failed");
        return;
    }
    updateDisplay("Starting...");

    // Inisialisasi Kamera
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;

    if (psramFound()) {
        config.frame_size = FRAMESIZE_UXGA;
        config.jpeg_quality = 10;
        config.fb_count = 2;
    } else {
        config.frame_size = FRAMESIZE_SVGA;
        config.jpeg_quality = 12;
        config.fb_count = 1;
    }

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed with error 0x%x", err);
        return;
    }

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    updateDisplay("WiFi Connected");

    startCameraServer();
}

void loop() {
    long distance = getDistance();
    Serial.print("Distance: ");
    Serial.println(distance);

    if (distance <= DISTANCE_THRESHOLD) {
        updateDisplay("Detecting Face...");
        
        // Simulasi Pengenalan Wajah
        matchFace = random(0, 2); // 0 = Tidak dikenali, 1 = Dikenali

        if (matchFace) {
            Serial1.println("OPEN");
            updateDisplay("Access Granted");
        } else {
            Serial1.println("DENY");
            updateDisplay("Access Denied");
        }
    }
    delay(2000);
}