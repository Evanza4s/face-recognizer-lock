#include <Servo.h>

Servo lockServo;
#define SERVO_PIN 9
#define SERVO_LOCKED 0
#define SERVO_UNLOCKED 90

void setup() {
    Serial.begin(115200);
    lockServo.attach(SERVO_PIN);
    lockServo.write(SERVO_LOCKED);
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    if (Serial.available()) {
        String response = Serial.readStringUntil('\n');
        response.trim();

        if (response == "recognized") {
            lockServo.write(SERVO_UNLOCKED);
            digitalWrite(LED_BUILTIN, HIGH);
            delay(5000);
            lockServo.write(SERVO_LOCKED);
            digitalWrite(LED_BUILTIN, LOW);
        }
    }
}
