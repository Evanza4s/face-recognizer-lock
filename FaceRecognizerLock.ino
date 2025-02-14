#include "esp_camera.h"
#include <WiFi.h>
#include <ESP32Servo.h>
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

// Buzzer & LED Pins
#define BUZZER_PIN 15
#define LED_GREEN 2
#define LED_RED 4

// Ultrasonic sensor pins
#define TRIG_PIN 12
#define ECHO_PIN 13
#define DISTANCE_THRESHOLD 50

// Servo configuration
Servo lockServo;
#define SERVO_PIN 14
#define SERVO_LOCKED_ANGLE 0
#define SERVO_UNLOCKED_ANGLE 90

// OLED Display Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

boolean matchFace = false;
boolean activeLock = false;
long prevMillis = 0;
int interval = 5000;

void startCameraServer();

// Function to measure distance
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

// Function to update OLED Display
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
    Serial.setDebugOutput(true);
    Serial.println();

    // Initialize OLED Display
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("SSD1306 initialization failed");
        return;
    }
    updateDisplay("Starting...");

    // Configure ultrasonic sensor
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    // Configure Buzzer & LEDs
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_RED, OUTPUT);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_RED, LOW);

    // Configure servo motor
    lockServo.attach(SERVO_PIN);
    lockServo.write(SERVO_LOCKED_ANGLE);

    // Camera initialization
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

    // Check if PSRAM is available
    if (psramFound()) {
        config.frame_size = FRAMESIZE_UXGA;
        config.jpeg_quality = 10;
        config.fb_count = 2;
    } else {
        config.frame_size = FRAMESIZE_SVGA;
        config.jpeg_quality = 12;
        config.fb_count = 1;
    }

    // Initialize camera
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
    Serial.println("\nWiFi connected");
    updateDisplay("WiFi OK");

    startCameraServer();
    Serial.print("Camera Ready! Use 'http://");
    Serial.print(WiFi.localIP());
    Serial.println("' to connect");
}

void loop() {
    // Measure distance
    long distance = getDistance();
    Serial.print("Distance: ");
    Serial.println(distance);

    // Cek apakah wajah cocok dan berada dalam jarak yang ditentukan
    if (matchFace && distance <= DISTANCE_THRESHOLD && !activeLock) {
        activeLock = true;
        lockServo.write(SERVO_UNLOCKED_ANGLE); // Unlock
        digitalWrite(LED_GREEN, HIGH);
        digitalWrite(LED_RED, LOW);
        tone(BUZZER_PIN, 1000, 500); // Beep sebentar
        updateDisplay("Access Granted");
        Serial.println("Face Matched: Unlocking...");

        prevMillis = millis();
    } 
    else if (!matchFace && distance <= DISTANCE_THRESHOLD) {
        digitalWrite(LED_GREEN, LOW);
        digitalWrite(LED_RED, HIGH);
        tone(BUZZER_PIN, 2000, 1000); // Beep lebih lama untuk ditolak
        updateDisplay("Access Denied");
        Serial.println("Face Not Recognized: Access Denied");
    }

    // Kunci kembali setelah interval
    if (activeLock && millis() - prevMillis > interval) {
        activeLock = false;
        matchFace = false;
        lockServo.write(SERVO_LOCKED_ANGLE);
        digitalWrite(LED_GREEN, LOW);
        digitalWrite(LED_RED, LOW);
        updateDisplay("Locked");
        Serial.println("Locking the lock...");
    }

    delay(500);
}