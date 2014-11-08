#include <Adafruit_NeoPixel.h>

#define NEOPIXEL_INPUT 6
uint8_t PWR = 28; // intensity: 0..255

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, NEOPIXEL_INPUT, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

uint32_t magenta = strip.Color(PWR, 0, PWR);
uint32_t cyan = strip.Color(0, PWR, PWR);
uint32_t blue = strip.Color(0, 0, PWR);
uint32_t red = strip.Color(PWR, 0, 0);
uint32_t orange = strip.Color(PWR, floor(PWR/2), 0);
uint32_t yellow = strip.Color(PWR, PWR, 0);
uint32_t black = strip.Color(0,0,0);

// actual values are stored here
uint8_t hygro;
uint8_t temp;

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  hygro = 0;
}

void loop() {
  lightHygroPixels();
  delay(2000);
}

void lightHygroPixels() {
  blackOut();
  delay(50);
  
  for (uint8_t i = 0; i<8; i++) {
     if (i*10 < hygro) {
        setHygroPixel(i, true);
     } else if (i == 0) {
       setHygroPixel(i, true); // always turn on the lowest pixel
     } else {
        setHygroPixel(i, false); 
     }
     strip.show();
     delay(50);
  }
}

void setHygroPixel(uint8_t v, bool on) {
  v = constrain(v, 0, 7);
  uint8_t i = 15 - v;
  
  if (on) {
    if (v < 3) {
      strip.setPixelColor(i, magenta);
    } else if (v < 5) {
      strip.setPixelColor(i, cyan);
    } else {
      strip.setPixelColor(i, blue);
    }
  } else {
    strip.setPixelColor(i, black); 
  }
}

void blackOut() {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, black);
  }
  strip.show();
}

