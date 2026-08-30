#include <Arduino.h>

int ledPin = 27;
int buttonPin = 4;
int pwmLedChannel = 0;

int ledState = 0;
int buttonState = HIGH;
int lastButtonState = HIGH;

unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

unsigned long previousLedMillis = 0;
int currentyDuty = 0;
int fadeStep = 5;

void handleButton();
void changeLedWriteByState();


void setup() {
  ledcSetup(pwmLedChannel, 5000, 10);
  ledcAttachPin(ledPin, pwmLedChannel);
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  handleButton();
  changeLedWriteByState();
}

void handleButton() {
  int innerButtonState = digitalRead(buttonPin);

  if (innerButtonState != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (innerButtonState != buttonState) {
      buttonState = innerButtonState;

      if (buttonState == LOW) {
        ledState++;
        if (ledState > 4) {
          ledState = 0;
        }
      }
    }
  }
  lastButtonState = innerButtonState;
}

void changeLedWriteByState() {
  unsigned long currentMillis = millis();
  switch (ledState) {
  case 0:
    ledcWrite(pwmLedChannel, 0);
    break;
  case 1:
    ledcWrite(pwmLedChannel, 1023);
    break;
  case 2:
    if (currentMillis - previousLedMillis >= 1000) {
      previousLedMillis = currentMillis;
      currentyDuty = (currentyDuty == 0) ? 1023 : 0;
      ledcWrite(pwmLedChannel, currentyDuty);
    }
    break;
  case 3:
    if (currentMillis - previousLedMillis >= 500) {
      previousLedMillis = currentMillis;
      currentyDuty = (currentyDuty == 0) ? 1023 : 0;
      ledcWrite(pwmLedChannel, currentyDuty); 
    }
    break;
  case 4:
    if (currentMillis - previousLedMillis >= 5) {
      previousLedMillis = currentMillis;
      currentyDuty += fadeStep;

      if (currentyDuty >= 1023) {
        currentyDuty = 1023;
        fadeStep = -fadeStep;
      } else if (currentyDuty <= 0) {
        currentyDuty = 0;
        fadeStep = -fadeStep;
      }

      ledcWrite(pwmLedChannel, currentyDuty);
    }
    break;
  }
}

/*
  If I just want to turn on a simple LED, I have to declare which GPIO pin will be used, call
  pinMode(usedGpio, OUTPUT) in setup(), and then call digitalWrite(usedGpio, HIGH). This sets the pin's
  output voltage to its HIGH level. For example:

  int ledPin = 27;

  void setup() {
    pinMode(ledPin, OUTPUT);
  }

  void loop() {
    digitalWrite(ledPin, HIGH);
  }

  --------------------------------------------------------------------------------------------------------------------------

  If I want to blink an LED, I have to alternate between the HIGH and LOW states using digitalWrite().
  For example:

  int ledPin = 27;

  void setup() {
    pinMode(ledPin, OUTPUT);
  }

  void loop() {
    digitalWrite(ledPin, HIGH);
    delay(500);
    digitalWrite(ledPin, LOW);
    delay(500);
  }

  --------------------------------------------------------------------------------------------------------------------------

  If I want to create a fading effect, I can use PWM (Pulse Width Modulation). I call ledcSetup(),
  passing the PWM channel (from 0 to 15), the desired frequency in hertz, and the resolution in bits.
  I also have to call ledcAttachPin() in setup(), passing the pin and the channel.
  After that, I can control the fading with a for loop by calling ledcWrite() with the channel and
  the duty cycle.

  The frequency in hertz (Hz) defines how many PWM cycles occur per second. At a low frequency, such
  as 30 Hz, the human eye may perceive flickering. At a sufficiently high frequency, the individual
  cycles are no longer noticeable, and changing the duty cycle creates the fading effect.

  The resolution in bits defines the number of available duty-cycle levels and therefore affects the
  smoothness of the transition:
  8 bits (2^8 levels): maximum duty cycle = 255
  10 bits (2^10 levels): maximum duty cycle = 1023
  12 bits (2^12 levels): maximum duty cycle = 4095

  #include <Arduino.h>

  int ledPin = 27;
  int redPinPwmChannel = 0;

  void setup() {
    ledcSetup(redPinPwmChannel, 5000, 10);
    ledcAttachPin(ledPin, redPinPwmChannel);
  }

  void loop() {
    for (int dutyCycle = 0; dutyCycle <= 1023; dutyCycle++) {
      ledcWrite(redPinPwmChannel, dutyCycle);  
      delay(10);
    }

    for (int dutyCycle = 1023; dutyCycle >= 0; dutyCycle--) {
      ledcWrite(redPinPwmChannel, dutyCycle);  
      delay(10);
    }
}

--------------------------------------------------------------------------------------------------------------------------

If I want to connect a button to control a LED, I just want to capture a INPUT_PULLUP event and controle the LED on or off with that.

#include <Arduino.h>

int ledPin = 27;
int buttonPin = 4;


void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  int buttonState = digitalRead(buttonPin);
  if (buttonState == 0) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
}

*/
