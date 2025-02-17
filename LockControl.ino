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

void updateDisplay(String message) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 20);
    display.println(message);
    display.display();
}

void setup() {
    Serial.begin(9600);
    lockServo.attach(SERVO_PIN);
    lockServo.write(0); // Posisi awal terkunci

    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_RED, OUTPUT);

    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_RED, LOW);

    // Inisialisasi OLED
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
            lockServo.write(90); // Buka kunci
            tone(BUZZER_PIN, 1000, 500);
            updateDisplay("Door Unlocked");
            Serial.println("Door Unlocked");
        }
        else if (receivedData == "DENY") {
            digitalWrite(LED_GREEN, LOW);
            digitalWrite(LED_RED, HIGH);
            tone(BUZZER_PIN, 2000, 1000);
            updateDisplay("Access Denied");
            Serial.println("Access Denied");
        }

        delay(5000); // Tunggu sebelum mengunci kembali
        lockServo.write(0); // Kunci kembali
        digitalWrite(LED_GREEN, LOW);
        digitalWrite(LED_RED, LOW);
        updateDisplay("Locked");
        Serial.println("Locked");
    }
}
