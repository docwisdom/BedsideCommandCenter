//pixel def
#include <Adafruit_NeoPixel.h>
#define pixelPIN 3
#define bluePIN 11
Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, pixelPIN, NEO_GRB + NEO_KHZ800);


void setup() {
  pinMode(bluePIN, OUTPUT);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  // Some example procedures showing how to display to the pixels:
  colorSet(strip.Color(255, 200, 200), 255); // Red
  analogWrite(bluePIN, 100);
}

// Fill the dots one after the other with a color
void colorSet(uint32_t c, uint8_t bright) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.setBrightness(bright);
      strip.show();
  }
}

