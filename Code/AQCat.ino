#include <Adafruit_SCD30.h>
#include <Adafruit_NeoPixel.h>
#include <ChainableLED.h>

// NeoPixel LED Strip
#define LED_PIN     12
#define NUMPIXELS   10
Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Chainable RGB LED (Katzenaugen)
#define DATA_PIN    11
#define CLOCK_PIN   10
#define NUM_EYES    2
ChainableLED eyes(DATA_PIN, CLOCK_PIN, NUM_EYES);

// SCD30
Adafruit_SCD30 scd30;

// Normale Farbe setzen
void setColor(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }
  pixels.show();
  for (int i = 0; i < NUM_EYES; i++) {
    eyes.setColorRGB(i, r, g, b);
  }
}

// Rot-Alarm: Strip von unten nach oben + Augen blinken
void redAlert() {
  // Strip von unten nach oben auffüllen
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 0, 0));
    pixels.show();
    delay(80);
  }

  // Augen 3x blinken
  for (int b = 0; b < 3; b++) {
    // Augen an
    for (int i = 0; i < NUM_EYES; i++) eyes.setColorRGB(i, 255, 0, 0);
    delay(300);
    // Augen aus
    for (int i = 0; i < NUM_EYES; i++) eyes.setColorRGB(i, 0, 0, 0);
    delay(300);
  }

  // Strip wieder löschen
  pixels.clear();
  pixels.show();
}

void setup() {
  Serial.begin(115200);

  pixels.begin();
  pixels.setBrightness(50);
  setColor(0, 0, 0);

  if (!scd30.begin()) {
    Serial.println("SCD30 nicht gefunden!");
    while (1);
  }
  Serial.println("SCD30 bereit!");
}

void loop() {
  if (scd30.dataReady()) {
    scd30.read();

    float co2 = scd30.CO2;

    Serial.print("CO2: "); Serial.print(co2); Serial.println(" ppm");
    Serial.print("Temp: "); Serial.print(scd30.temperature); Serial.println(" °C");
    Serial.print("Humidity: "); Serial.print(scd30.relative_humidity); Serial.println(" %");

    if (co2 < 800) {
      setColor(0, 0, 255);
      Serial.println("Luft gut ✓");
    } else if (co2 < 1000) {
      setColor(255, 165, 0);
      Serial.println("Bald lüften!");
    } else {
      Serial.println("JETZT LÜFTEN!");
      redAlert();
    }
  }
  delay(1000);
}