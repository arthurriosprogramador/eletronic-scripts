#include <Wire.h>
#include <WiFi.h>
#include <time.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BMP280.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

const char* ssid = "wifi-name";
const char* password = "wifi-password";

const char* ntpServer = "a.st1.ntp.br";
const long  gmtOffset_sec      = -3 * 3600;
const int   daylightOffset_sec = 0;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_BMP280 bmp;

void setup() {
  Serial.begin(115200);

  Wire.begin(21, 22);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Falha ao inicializar o OLED. Verifique o endereço ou conexões."));
    for(;;);
  }

  if (!bmp.begin(0x76)) {
    Serial.println(F("Nao foi possivel encontrar o BMP280! Verifique as conexoes."));
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 20);
    display.println(F("Erro: BMP280 off"));
    display.display();
    for(;;);
  }

  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     
                  Adafruit_BMP280::SAMPLING_X2,     
                  Adafruit_BMP280::SAMPLING_X16,    
                  Adafruit_BMP280::FILTER_X16,      
                  Adafruit_BMP280::STANDBY_MS_500);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 25);
  display.println(F("Conectando Wi-Fi..."));
  display.display();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(F("\nWi-Fi Conectado!"));

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) {
    delay(200);
  }
}

void loop() {
  float temperature = bmp.readTemperature();

  struct tm timeinfo;
  char watchBuffer[6] = "--:--";

  if (getLocalTime(&timeinfo)) {
    strftime(watchBuffer, sizeof(watchBuffer), "%H:%M", &timeinfo);
  }

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(95, 2);
  display.print(watchBuffer);

  display.setTextSize(2);
  display.setCursor(18, 26);
  display.print(temperature, 1);
  display.print((char)247);
  display.print("C");

  display.display();

  delay(1000);
}

/*
  Change the "wifi-name" and "wifi-password" to the real one.

  To power the circuit, I need to connect 3V3 from the ESP32 to the red (+) rail on the breadboard.
  I need to connect GND from the ESP32 to the blue (-) rail on the breadboard.

  To power the OLED display and the BMP280 sensor, I need to connect both VCC pins to the red (+) rail,
  and both GND pins to the blue (-) rail.

  Both the display and the sensor use the I2C bus, so they share the same communication wires.
  I need to connect the SDA pin of the display and the SDA pin of the sensor to GPIO 21 on the ESP32.
  I need to connect the SCL pin of the display and the SCL pin of the sensor to GPIO 22 on the ESP32.

  To force the BMP280 sensor into I2C mode, I need to connect its CSB pin to 3.3 V (the red rail).
  To set the BMP280 sensor I2C address to 0x76, I need to connect its SDO pin to GND (the blue rail).

  The ESP32 connects to my home Wi-Fi and gets the exact local time from an NTP server over the internet.
  The code reads the temperature from the BMP280 sensor and reads the current time from the ESP32 internal clock.
  The code then clears the OLED screen and draws the time on the top-right and the temperature in big text in the center.
*/

