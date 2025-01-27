#include "esp_camera.h"
#include <WiFi.h>
#include <ESP32Servo.h>

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

// Ultrasonic sensor pins
#define TRIG_PIN 12
#define ECHO_PIN 13
#define DISTANCE_THRESHOLD 50

// Servo configuration
Servo lockServo;
#define SERVO_PIN 14
#define SERVO_LOCKED_ANGLE 0
#define SERVO_UNLOCKED_ANGLE 90

boolean matchFace = false;
boolean activeLock = false;
long prevMillis = 0;
int interval = 5000;

void startCameraServer();

// Function to measure distance using ultrasonic sensor
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

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  // Configure ultrasonic sensor
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Configure servo motor
  lockServo.attach(SERVO_PIN);
  lockServo.write(SERVO_LOCKED_ANGLE);

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

  // Initialize with high specs to pre-allocate larger buffers
  if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // Camera initialization
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t *s = esp_camera_sensor_get();
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);       // Flip it back
    s->set_brightness(s, 1); // Increase brightness
    s->set_saturation(s, -2); // Lower saturation
  }

  s->set_framesize(s, FRAMESIZE_QVGA);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

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

  // Check if face matches and person is within range
  if (matchFace && distance <= DISTANCE_THRESHOLD && !activeLock) {
    activeLock = true;
    lockServo.write(SERVO_UNLOCKED_ANGLE); // Unlock the lock
    Serial.println("Face matched and person detected. Unlocking lock...");
    prevMillis = millis();
  }

  if (distance <= DISTANCE_THRESHOLD && activate ) {
    activeLock = true;
    prevMIllis = millis();
  }
  delay(10000);

  // Lock after the interval
  if (activeLock && millis() - prevMillis > interval) {
    activeLock = false;
    matchFace = false;
    lockServo.write(SERVO_LOCKED_ANGLE); // Lock again
    Serial.println("Locking the lock...");
  }

  delay(10000);
}
