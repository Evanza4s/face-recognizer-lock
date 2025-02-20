#include <Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Pin komponen
#define SERVO_PIN 9
#define BUZZER_PIN 10
#define LED_GREEN 6
#define LED_RED 7

Servo lockServo;
String receivedData = "";

// OLED Display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void updateDisplay(String message);

void setup() {
    Serial.begin(9600);
    lockServo.attach(SERVO_PIN);
    lockServo.write(0);

    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_RED, OUTPUT);

    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_RED, LOW);

    // OLED Display
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("SSD1306 initialization failed");
        return;
    }
    updateDisplay("System Ready");
}

void loop() {
    if (Serial.available()) {
        receivedData = Serial.readStringUntil('\n');
        receivedData.trim();

        if (receivedData == "OPEN") {
            digitalWrite(LED_GREEN, HIGH);
            digitalWrite(LED_RED, LOW);
            lockServo.write(90);
            tone(BUZZER_PIN, 1000, 500);
            updateDisplay("Door Unlocked");
            Serial.println("Door Unlocked");
        } else if (receivedData == "SLEEP") {
            digitalWrite(LED_GREEN, LOW);
            digitalWrite(LED_RED, LOW);
            lockServo.write(0);
            updateDisplay("System in Sleep Mode");
            Serial.println("System in Sleep Mode");
        } else if (receivedData == "DENY") {
            digitalWrite(LED_GREEN, LOW);
            digitalWrite(LED_RED, HIGH);
            tone(BUZZER_PIN, 2000, 1000);
            updateDisplay("Access Denied");
            Serial.println("Access Denied");
        }
    }
}
