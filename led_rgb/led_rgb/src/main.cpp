#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// Mapping verified by the colors produced by this specific RGB LED.
const int redPin = 27;
const int greenPin = 25;
const int bluePin = 26;
const int potPin = 34;

const char *deviceName = "LED RGB ESP32";
const char *serviceUuid = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
const char *colorCharacteristicUuid = "beb5483e-36e1-4688-b7f5-ea07361b26a8";

int selectedRed = 255;
int selectedGreen = 0;
int selectedBlue = 0;
bool deviceConnected = false;

void setColor(int redColor, int greenColor, int blueColor);

class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *server) override {
    deviceConnected = true;
  }

  void onDisconnect(BLEServer *server) override {
    deviceConnected = false;
    BLEDevice::startAdvertising();
  }
};

class ColorCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *characteristic) override {
    String value = characteristic->getValue().c_str();

    if (value.length() == 7 && value[0] == '#') {
      selectedRed = strtol(value.substring(1, 3).c_str(), nullptr, 16);
      selectedGreen = strtol(value.substring(3, 5).c_str(), nullptr, 16);
      selectedBlue = strtol(value.substring(5, 7).c_str(), nullptr, 16);
      Serial.printf(
          "Cor BLE: %s | R: %d | G: %d | B: %d\n",
          value.c_str(), selectedRed, selectedGreen, selectedBlue);
    }
  }
};

void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  Serial.begin(115200);

  BLEDevice::init(deviceName);
  BLEServer *server = BLEDevice::createServer();
  server->setCallbacks(new ServerCallbacks());

  BLEService *service = server->createService(serviceUuid);
  BLECharacteristic *colorCharacteristic = service->createCharacteristic(
      colorCharacteristicUuid,
      BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR);
  colorCharacteristic->setCallbacks(new ColorCallbacks());
  colorCharacteristic->addDescriptor(new BLE2902());
  service->start();

  BLEAdvertising *advertising = BLEDevice::getAdvertising();
  advertising->addServiceUUID(serviceUuid);
  advertising->setScanResponse(true);
  BLEDevice::startAdvertising();
  Serial.println("Bluetooth pronto: procure por LED RGB ESP32");
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

  setColor(
      selectedRed * brightness / 255,
      selectedGreen * brightness / 255,
      selectedBlue * brightness / 255);
  delay(20);
}

void setColor(int redColor, int greenColor, int blueColor) {
  analogWrite(redPin, redColor);
  analogWrite(greenPin, greenColor);
  analogWrite(bluePin, blueColor);
}

/*
  To use a common-cathode RGB LED, I need to connect its longest leg (the common cathode) to GND.
  I need to connect each of the other legs to its own GPIO pin through a current-limiting resistor.

  To use a potentiometer, I need to connect one outer pin to 3.3 V and the other outer pin to GND.
  I need to connect the middle pin (the wiper) to an ADC-capable pin on the ESP32. This project uses GPIO 34.

  The ESP32 has built-in Bluetooth, so I do not need an external Bluetooth module or additional wiring.
  The ESP32 advertises its name and service UUID, allowing the web app to find it and connect to it.
  The app writes a color in the #RRGGBB format to the color characteristic, and the ESP32 uses that value
  to control the red, green, and blue channels of the LED.
*/
