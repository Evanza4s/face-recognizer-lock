#include <Servo.h>

// Pin komponen
#define SERVO_PIN 9
#define BUZZER_PIN 10
#define LED_GREEN 6
#define LED_RED 7

Servo lockServo;
String receivedData = "";

void setup() {
    Serial.begin(9600);
    lockServo.attach(SERVO_PIN);
    lockServo.write(0); // Posisi awal terkunci

    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_RED, OUTPUT);

    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_RED, LOW);
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
            Serial.println("Door Unlocked");
        }
        else if (receivedData == "DENY") {
            digitalWrite(LED_GREEN, LOW);
            digitalWrite(LED_RED, HIGH);
            tone(BUZZER_PIN, 2000, 1000);
            Serial.println("Access Denied");
        }

        delay(5000); // Delay untuk simpan status sebelum kembali ke posisi awal
        lockServo.write(0); // Kunci kembali
        digitalWrite(LED_GREEN, LOW);
        digitalWrite(LED_RED, LOW);
    }
}
