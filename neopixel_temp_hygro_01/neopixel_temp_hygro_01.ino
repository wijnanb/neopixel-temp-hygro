#include <Adafruit_NeoPixel.h>

#define NEOPIXEL_INPUT 6
uint8_t PWR = 30; // intensity: 0..255

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

uint32_t magenta = strip.Color(255, 0, 255);
uint32_t cyan = strip.Color(0, 255, 255);
uint32_t blue = strip.Color(0, 0, 255);
uint32_t red = strip.Color(255, 0, 0);
uint32_t orange = strip.Color(255, 127, 0);
uint32_t yellow = strip.Color(255, 255, 0);
uint32_t greellow = strip.Color(127, 255, 0);
uint32_t black = strip.Color(0,0,0);

uint8_t colors[16][3] = {
  {127, 255, 0}, // temp 0: greellow
  {127, 255, 0}, // temp 1: greellow
  {255, 255, 0}, // temp 2: yellow
  {255, 255, 0}, // temp 3: yellow
  {255, 127, 0}, // temp 4: orange
  {255, 127, 0}, // temp 5: orange
  {255, 0, 0},  // temp 6: red
  {255, 0, 0},  // temp 7: red
  {0, 0, 255},  // hygro 7: blue
  {0, 0, 255},  // hygro 6: blue
  {0, 0, 255},  // hygro 5: blue
  {0, 255, 255}, // hygro 4: cyan
  {0, 255, 255}, // hygro 3: cyan
  {255, 0, 255}, // hygro 2: magenta
  {255, 0, 255}, // hygro 1: magenta
  {255, 0, 255}  // hygro 0: magenta
};



int anim = 70;
int fade_time = 70;

// actual values are stored here
uint8_t hygro;
float temp;

int cycle_count;

void setup() {
  randomSeed(analogRead(0));
  
  strip.begin();
  strip.setBrightness(PWR);
  strip.show(); // Initialize all pixels to 'off'
  
  setRandomValues();
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
  setRandomValues();
  displayOn();
}

void lightPixels() {
  for (uint8_t i = 0; i<8; i++) {
    
    bool hygroOn = isHygroOn(i);
    bool tempOn = isTempOn(i);
    
    setHygroPixel(i, hygroOn);
    setTempPixel(i, tempOn);
    
    strip.show();
    delay(anim);
  }
}

bool isHygroOn(int i) {
  i = constrain(i, 0, 7);
  return (i*10 < hygro) || i == 0;
}

bool isTempOn(int i) {
  i = constrain(i, 0, 7);
  return (i < temp-18) || i == 0;
}

bool isOn(int i) {
  i = constrain(i, 0, 15);
  
  if (i > 7) {
    return isHygroOn(15-i);
  } else {
    return isTempOn(i);
  }
}

void setHygroPixel(uint8_t v, bool on) {
  v = constrain(v, 0, 7);
  uint8_t i = 15 - v;
  
  if (on) {
    strip.setPixelColor(i, strip.Color(colors[i][0], colors[i][1], colors[i][2]));
  } else {
    strip.setPixelColor(i, black); 
  }
}

void setTempPixel(uint8_t i, bool on) {
  i = constrain(i, 0, 7);

  if (on) {
    strip.setPixelColor(i, strip.Color(colors[i][0], colors[i][1], colors[i][2]));
  } else {
    strip.setPixelColor(i, black); 
  }
}

void blinkActivePixelsAndDisplayOff(int duration) {
   int cycle = 350;
   
   uint8_t activeHygroPixel = 15 - constrain(floor((hygro-1)/10), 0, 7);
   uint8_t activeTempPixel = constrain(floor((temp-1)-18), 0, 7);
   
   cycle_count = 0;
   while (cycle_count*cycle*2 < duration) {
     // fadeout
     for (int i = 0; i<256; i++) {
       uint8_t r_hygro = max(0, colors[activeHygroPixel][0] - colors[activeHygroPixel][0]*i/255);
       uint8_t g_hygro = max(0, colors[activeHygroPixel][1] - colors[activeHygroPixel][1]*i/255);
       uint8_t b_hygro = max(0, colors[activeHygroPixel][2] - colors[activeHygroPixel][2]*i/255);
       strip.setPixelColor(activeHygroPixel, strip.Color(r_hygro, g_hygro, b_hygro));
       
       uint8_t r_temp = max(0, colors[activeTempPixel][0] - colors[activeTempPixel][0]*i/255);
       uint8_t g_temp = max(0, colors[activeTempPixel][1] - colors[activeTempPixel][1]*i/255);
       uint8_t b_temp = max(0, colors[activeTempPixel][2] - colors[activeTempPixel][2]*i/255);
       strip.setPixelColor(activeTempPixel, strip.Color(r_temp, g_temp, b_temp));
       
       strip.show();
       delay(2);
     }
     
     delay(30);
     
     // fadein
     for (int i = 0; i<256; i++) {
       uint8_t r_hygro = min(colors[activeHygroPixel][0]*i/255, colors[activeHygroPixel][0]);
       uint8_t g_hygro = min(colors[activeHygroPixel][1]*i/255, colors[activeHygroPixel][1]);
       uint8_t b_hygro = min(colors[activeHygroPixel][2]*i/255, colors[activeHygroPixel][2]);
       strip.setPixelColor(activeHygroPixel, strip.Color(r_hygro, g_hygro, b_hygro));
       
       uint8_t r_temp = min(colors[activeTempPixel][0]*i/255, colors[activeTempPixel][0]);
       uint8_t g_temp = min(colors[activeTempPixel][1]*i/255, colors[activeTempPixel][1]);
       uint8_t b_temp = min(colors[activeTempPixel][2]*i/255, colors[activeTempPixel][2]);
       strip.setPixelColor(activeTempPixel, strip.Color(r_temp, g_temp, b_temp));
       
       strip.show();
       delay(1);
     }
     
     delay(180);
     
     cycle_count++;  
   }
   
   displayOff();
}

void blackOut() {
  delay(50);
  
  uint8_t r, g, b;
  // fadeout
  for (int f = 0; f<256; f++) {
    for(uint16_t i=0; i<strip.numPixels(); i++) {
      if (isOn(i)) {
         r = max(0, colors[i][0] - f);
         g = max(0, colors[i][1] - f);
         b = max(0, colors[i][2] - f);
         strip.setPixelColor(i, strip.Color(r, g, b));
      }
     }
     
     strip.show();
     delay(2);
  }
}

void setRandomValues() {
  hygro = random(0, 90); // 0..90
  temp = random(180, 260) / 10; // 18.0..26.0
}

