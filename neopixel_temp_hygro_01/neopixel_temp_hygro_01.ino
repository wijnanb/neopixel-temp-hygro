#include <DHT.h>
#include <Adafruit_NeoPixel.h>

#define DHTPIN 2
#define NEOPIXEL_INPUT 6
#define BUTTON_PIN 7
#define BUTTON_LED_PIN 13

#define DHTTYPE DHT22   // DHT 22 temp/hygro sensor  (AM2302)

#define DEMO 0

int ledState = HIGH;
int buttonState;
int lastButtonState = LOW;
long lastDebounceTime = 0;
long debounceDelay = 50;
long buttonDownTime = 0;
long longPressTime = 1000;

uint8_t PWR = 255; // intensity: 0..255


// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);
// NOTE: For working with a faster chip, like an Arduino Due or Teensy, you
// might need to increase the threshold for cycle counts considered a 1 or 0.
// You can do this by passing a 3rd parameter for this threshold.  It's a bit
// of fiddling to find the right value, but in general the faster the CPU the
// higher the value.  The default for a 16mhz AVR is a value of 6.  For an
// Arduino Due that runs at 84mhz a value of 30 works.
// Example to initialize DHT sensor for Arduino Due:
//DHT dht(DHTPIN, DHTTYPE, 30);

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
int same_color_index = -1;
bool countdown_mode = false;
bool receiving_input = true;

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));
  
  pinMode(BUTTON_PIN, INPUT);
  pinMode(BUTTON_LED_PIN, OUTPUT);
  pinMode(DHTPIN, INPUT);
  
  Serial.println("DHT22 begin");
  dht.begin();
  
  strip.begin();
  strip.setBrightness(PWR);
  strip.show(); // Initialize all pixels to 'off'
  
  if (DEMO) {
    if (countdown_mode) {
      enterCountdownMode();
    } else {
      setRandomValues();
      displayOn();
    }
  }
}

void loop() {
  readButtonInput();
}

void readButtonInput() {
  if (!receiving_input) {
    return;
  }
  int reading = digitalRead(BUTTON_PIN);
  
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  } 
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == HIGH) {
        onButtonDown();
      } else {
        onButtonUp();
      }
    } else if (reading == HIGH && buttonDownTime != 0) {
       // detect longPress before buttonUp event
       if (millis()-buttonDownTime >= longPressTime) {
         onLongButtonPress();
         buttonDownTime = 0;
       }
    }
  }
  
  lastButtonState = reading; 
}

void onButtonDown() {
  buttonDownTime = millis();
  digitalWrite(BUTTON_LED_PIN, HIGH);
}

void onButtonUp() {
  digitalWrite(BUTTON_LED_PIN, LOW);
  
  long buttonUpTime = millis();
  if (buttonUpTime-buttonDownTime < longPressTime) {
    onShortButtonPress();
  }
  
  buttonDownTime = 0;
}

void onShortButtonPress() {
  if (countdown_mode) {
    onCountdownShortButtonPress();
  } else {
    //setRandomValues();
    readFromSensor();
    displayOn();
  }
}

void onLongButtonPress() {
  if (countdown_mode) {
    onCountdownLongButtonPress();
  } else {
    enterCountdownMode();
  }
}

void displayOn() {
   receiving_input = false;
   lightPixels();
   blinkActivePixelsAndDisplayOff(3000);
}

void displayOff() {
  blackOut();
  
  receiving_input = true;
  
  if (DEMO) {
    delayWithInput(3000);
    setRandomValues();
    displayOn();
  } else {
     // listen for button press in loop
  }
}

void lightPixels() {
  // animate from 0 to 8
  for (uint8_t i = 0; i<8; i++) {  
    if (isTempOn(i) && isHygroOn(i)) {
      int pixels[2] = {i, (15-i)};
      fadeIn(pixels, 2, -5);
    } else if (isTempOn(i)) {
      int pixels[1] = {i};
      fadeIn(pixels, 1, -5);
    } else if (isHygroOn(i)) {
      int pixels[1] = {(15-i)};
      fadeIn(pixels, 1, -5);
    }
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
  if (countdown_mode) {
    return true;
  } else {
    i = constrain(i, 0, 15);
    
    if (i > 7) {
      return isHygroOn(15-i);
    } else {
      return isTempOn(i);
    }
  }
}

void blinkActivePixelsAndDisplayOff(int duration) {
   
   uint8_t activeHygroPixel = 15 - constrain(floor((hygro-1)/10), 0, 7);
   uint8_t activeTempPixel = constrain(floor((temp-1)-18), 0, 7);
   int pixels[2] = {activeHygroPixel, activeTempPixel};
   
   int cycle = 350;  
   cycle_count = 0;
   while (cycle_count*cycle*2 < duration) {
     fadeOut(pixels, 2, 2);
     delayWithInput(30);
     fadeIn(pixels, 2, 1);
     delayWithInput(180);
     cycle_count++;  
   }
   
   displayOff();
}

void blackOut() {
  delay(50);
  
  int pixels[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  fadeOut(pixels, strip.numPixels(), 2);
  off();
}

void off() {
  for (int i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, black);
  }
  strip.show();
}

void fadeIn(int pixels[], int numPixels, int d) {
  fade(true, pixels, numPixels, d);
}

void fadeOut(int pixels[], int numPixels, int d) {
  fade(false, pixels, numPixels, d);
}

void fade(bool in, int pixels[], int numPixels, int d) {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t r_base;
  uint8_t g_base;
  uint8_t b_base;
  int inc;
  
  if (d < 0) {
    inc = -d;
    d = 1;
  } else {
    inc = 1;
  }
  
  for (int f=0; f<256; f+=inc) {
    for (int i=0; i<numPixels; i++) {
      if (isOn(i)) {
        int p = pixels[i];
        
        if (same_color_index == -1) {
          r_base = colors[p][0];
          g_base = colors[p][1];
          b_base = colors[p][2];
        } else {
          r_base = colors[same_color_index][0];
          g_base = colors[same_color_index][1];
          b_base = colors[same_color_index][2];
        }
        
        if (in) {
          r = min(r_base*f/255, r_base);
          g = min(g_base*f/255, g_base);
          b = min(b_base*f/255, b_base);
        } else {
          r = max(0, r_base - r_base*f/255);
          g = max(0, g_base - g_base*f/255);
          b = max(0, b_base - b_base*f/255);
        }
        strip.setPixelColor(p, strip.Color(r, g, b));
      }
    }
    strip.show();
    delayWithInput(d);
  }
}

void setRandomValues() {
  hygro = random(0, 90); // 0..90
  temp = random(180, 260) / 10; // 18.0..26.0
}

void readFromSensor() {
  float hygro_correction = 1.15; // plus 15%
  float temp_correction = 1.015; // plus 1.5%
  hygro = constrain(0, round(dht.readHumidity()*hygro_correction), 90);
  temp = constrain(18.0, dht.readTemperature()*temp_correction, 26.0);
  
  Serial.print("Humidity: "); 
  Serial.print(hygro) ;
  Serial.print(" %\t");
  Serial.print("Temperature: "); 
  Serial.print(temp);
  Serial.println(" *C ");
  
  if (isnan(hygro) || isnan(temp)) {
    Serial.println("Failed to read from DHT sensor!");
    hygro = 0;
    temp = 18.0;
  }
}

void delayWithInput(int d) {
   for (int i=0; i<d; i++) {
     readButtonInput();
     delay(1);
   } 
}

