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
uint32_t greellow = strip.Color(floor(PWR/2), PWR, 0);
uint32_t black = strip.Color(0,0,0);

int anim = 70;

// actual values are stored here
uint8_t hygro;
float temp;

int cycle_count;

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  hygro = 63;
  temp = 20.0;
  displayOn();
}

void loop() {
  
}

void displayOn() {
   lightPixels();
   blinkActivePixelsAndDisplayOff(5000);
}

void displayOff() {
  blackOut();
  
  // listen for button press 
  delay(2000);
  displayOn();
}

void lightPixels() {
  blackOut();
  delay(anim);
  
  for (uint8_t i = 0; i<8; i++) {
    bool hygroOn = (i*10 < hygro) || i == 0;
    bool tempOn = (i < temp-18) || i == 0;
    
    setHygroPixel(i, hygroOn);
    setTempPixel(i, tempOn);
    
    strip.show();
    delay(anim);
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

void setTempPixel(uint8_t i, bool on) {
  i = constrain(i, 0, 7);

  if (on) {
    if (i < 2) {
      strip.setPixelColor(i, greellow);
    } else if (i < 4) {
      strip.setPixelColor(i, yellow);
    } else if (i < 6) {
      strip.setPixelColor(i, orange);
    } else {
      strip.setPixelColor(i, red);
    }
  } else {
    strip.setPixelColor(i, black); 
  }
}

void blinkActivePixelsAndDisplayOff(int duration) {
   int cycle = 350;
   
   uint8_t activeHygroPixel = constrain(floor((hygro-1)/10), 0, 7);
   uint8_t activeTempPixel = constrain(floor((temp-1)-18), 0, 7);
   
   cycle_count = 0;
   while (cycle_count*cycle*2 < duration) {
     setHygroPixel(activeHygroPixel, false);
     setTempPixel(activeTempPixel, false);
     strip.show();
     delay(cycle);
     setHygroPixel(activeHygroPixel, true);
     setTempPixel(activeTempPixel, true);
     strip.show();
     delay(cycle);
     cycle_count++;  
   }
   
   displayOff();
}

void blackOut() {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, black);
  }
  strip.show();
}

