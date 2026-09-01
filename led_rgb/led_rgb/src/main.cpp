#include <Arduino.h>

const int redPin = 25;
const int greenPin = 26;
const int bluePin = 27;
const int potPin = 34;

void setColor(int redColor, int greenColor, int blueColor);

void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  // setColor(255, 0, 0);
  // delay(1000);
  // setColor(0, 255, 0);
  // delay(1000);
  // setColor(0, 0, 255);
  // delay(1000);
  // setColor(255, 255, 0);
  // delay(1000);
  // setColor(255, 0, 255);
  // delay(1000);
  // setColor(0, 255, 255);
  // delay(1000);
  // setColor(255, 255, 255);
  // delay(1000);
  // setColor(0, 0, 0);
  int potValue = analogRead(potPin);

  int brightness = map(potValue, 50, 4000, 0, 255);
  brightness = constrain(brightness, 0, 255);

  analogWrite(redPin, brightness);
  analogWrite(greenPin, 0);
  analogWrite(bluePin, 0);
  Serial.printf("ADC: %d | Brilho: %d\n", potValue, brightness);
  delay(20);
}

void setColor(int redColor, int greenColor, int blueColor) {
  analogWrite(redPin, redColor);
  analogWrite(greenPin, greenColor);
  analogWrite(bluePin, blueColor);
}