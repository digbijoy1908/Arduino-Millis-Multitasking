/*
 * Arduino-Millis-Multitasking
 *
 * Runs four tasks at the same time without using delay():
 *   1. Red LED    - blinks every 1 second
 *   2. Blue LED   - fades up and down (PWM)
 *   3. Yellow LED - ON while the push button is held
 *   4. Servo      - sweeps between 0 and 180 degrees
 *
 * Board: Arduino Uno / Nano
 * Library: Servo (built in)
 */

#include <Servo.h>

// ---------- Pin configuration ----------
const uint8_t RED_LED_PIN    = 11;
const uint8_t BLUE_LED_PIN   = 5;   // must be a PWM pin (~)
const uint8_t YELLOW_LED_PIN = 9;
const uint8_t BUTTON_PIN     = 7;   // button between pin and GND
const uint8_t SERVO_PIN      = 6;

// ---------- Timing (milliseconds) ----------
const unsigned long BLINK_INTERVAL  = 1000;
const unsigned long FADE_INTERVAL   = 10;
const unsigned long BUTTON_INTERVAL = 50;
const unsigned long SERVO_INTERVAL  = 15;

// ---------- Task state ----------
unsigned long lastBlinkTime  = 0;
unsigned long lastFadeTime   = 0;
unsigned long lastButtonTime = 0;
unsigned long lastServoTime  = 0;

bool redLedState = LOW;

int brightness = 0;
int fadeStep   = 1;

int servoAngle = 0;
int servoStep  = 1;

Servo myServo;

// ---------- Tasks ----------

// Toggle the red LED every BLINK_INTERVAL
void blinkRedLed(unsigned long currentMillis) {
  if (currentMillis - lastBlinkTime >= BLINK_INTERVAL) {
    lastBlinkTime = currentMillis;
    redLedState = !redLedState;
    digitalWrite(RED_LED_PIN, redLedState);
  }
}

// Fade the blue LED up and down
void fadeBlueLed(unsigned long currentMillis) {
  if (currentMillis - lastFadeTime >= FADE_INTERVAL) {
    lastFadeTime = currentMillis;

    brightness += fadeStep;

    // Reverse direction at the limits
    if (brightness >= 255) {
      brightness = 255;
      fadeStep = -fadeStep;
    } else if (brightness <= 0) {
      brightness = 0;
      fadeStep = -fadeStep;
    }

    analogWrite(BLUE_LED_PIN, brightness);
  }
}

// Yellow LED follows the button (INPUT_PULLUP: pressed = LOW)
void updateYellowLed(unsigned long currentMillis) {
  if (currentMillis - lastButtonTime >= BUTTON_INTERVAL) {
    lastButtonTime = currentMillis;

    bool buttonPressed = (digitalRead(BUTTON_PIN) == LOW);
    digitalWrite(YELLOW_LED_PIN, buttonPressed);
  }
}

// Sweep the servo between 0 and 180 degrees
void sweepServo(unsigned long currentMillis) {
  if (currentMillis - lastServoTime >= SERVO_INTERVAL) {
    lastServoTime = currentMillis;

    servoAngle += servoStep;

    // Reverse direction at the limits
    if (servoAngle >= 180) {
      servoAngle = 180;
      servoStep = -servoStep;
    } else if (servoAngle <= 0) {
      servoAngle = 0;
      servoStep = -servoStep;
    }

    myServo.write(servoAngle);
  }
}

// ---------- Arduino core ----------

void setup() {
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  myServo.attach(SERVO_PIN);
}

void loop() {
  unsigned long currentMillis = millis();

  blinkRedLed(currentMillis);
  fadeBlueLed(currentMillis);
  updateYellowLed(currentMillis);
  sweepServo(currentMillis);
}