#include <Arduino.h>
#include <TrafficLight.h>

//LED's settings
int redPin = 27;
int yellowPin = 25;
int greenPin = 33;

int pwmRedChannel = 0;
int pwmYellowChannel = 1;
int pwmGreenChannel = 2;

int defaultFreq = 5000;
int defaultBits = 10;
int activeDuty = 1023;
int inactiveDuty = 0;

unsigned long greenLightTime = 30000;
unsigned long minGreenLightTime = 15000;
unsigned long yellowLightTime = 4000;
unsigned long redLightTime = 26000;

TrafficLight currentTrafficLight = TrafficLight::GREEN;

unsigned long millisSinceLastLight = 0;
unsigned long lastLightInstant = 0;

//Button settings
int buttonPin = 4;

int previousButtonState = HIGH;
bool hasAcceptedClickBefore = false;

unsigned long buttonCooldown = 10000;
unsigned long millisSinceLastClick = 0;
unsigned long lastClickInstant = 0;

bool hasPedestrianRequestedPassage = false;

void manageTrafficLight();
void activateTrafficLight();
void handlePedestrianButton();

void setup() {
  ledcSetup(pwmRedChannel, defaultFreq, defaultBits);
  ledcAttachPin(redPin, pwmRedChannel);

  ledcSetup(pwmYellowChannel, defaultFreq, defaultBits);
  ledcAttachPin(yellowPin, pwmYellowChannel);

  ledcSetup(pwmGreenChannel, defaultFreq, defaultBits);
  ledcAttachPin(greenPin, pwmGreenChannel);

  pinMode(buttonPin, INPUT_PULLUP);

  activateTrafficLight();
}

void loop() {
  manageTrafficLight();
  handlePedestrianButton();
}

void handlePedestrianButton() {
  int buttonState = digitalRead(buttonPin);
  unsigned long currentMillis = millis();
  millisSinceLastClick = currentMillis - lastClickInstant; 

  bool hasButtonJustBeenPressed = previousButtonState == HIGH && buttonState == LOW;

  bool clickCooldownFinished = !hasAcceptedClickBefore || millisSinceLastClick >= buttonCooldown;

  if (hasButtonJustBeenPressed && currentTrafficLight == TrafficLight::GREEN && clickCooldownFinished) {
    hasPedestrianRequestedPassage = true;
    lastClickInstant = currentMillis;
    hasAcceptedClickBefore = true;
  }

  previousButtonState = buttonState;
}

void manageTrafficLight() {
  unsigned long currentMillis = millis();
  millisSinceLastLight = currentMillis - lastLightInstant;

  if (millisSinceLastLight >= yellowLightTime && currentTrafficLight == TrafficLight::YELLOW) {
    currentTrafficLight = TrafficLight::RED;
    activateTrafficLight();
    lastLightInstant = currentMillis;
  } else if (
    (millisSinceLastLight >= greenLightTime && currentTrafficLight == TrafficLight::GREEN) || 
  (hasPedestrianRequestedPassage == true && millisSinceLastLight >= minGreenLightTime && currentTrafficLight == TrafficLight::GREEN)
) {
    currentTrafficLight = TrafficLight::YELLOW;
    activateTrafficLight();
    lastLightInstant = currentMillis;
    hasPedestrianRequestedPassage = false;
  } else if (millisSinceLastLight >= redLightTime && currentTrafficLight == TrafficLight::RED) {
    currentTrafficLight = TrafficLight::GREEN;
    activateTrafficLight();
    lastLightInstant = currentMillis;
  }
  
}

void activateTrafficLight() {
  switch (currentTrafficLight) {
    case TrafficLight::RED:
      ledcWrite(pwmRedChannel, activeDuty);
      ledcWrite(pwmGreenChannel, inactiveDuty);
      ledcWrite(pwmYellowChannel, inactiveDuty);
      break;
    case TrafficLight::YELLOW:
      ledcWrite(pwmYellowChannel, activeDuty);
      ledcWrite(pwmGreenChannel, inactiveDuty);
      ledcWrite(pwmRedChannel, inactiveDuty);
      break;
    case TrafficLight::GREEN:
      ledcWrite(pwmGreenChannel, activeDuty);
      ledcWrite(pwmRedChannel, inactiveDuty);
      ledcWrite(pwmYellowChannel, inactiveDuty);
      break;
  }
}

