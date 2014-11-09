
int minutes;
bool interrupted = false;
bool activate_exit = false;

void enterCountdownMode() {
  countdown_mode = true;
  same_color_index = 2;
   // animate from 0 to 16
  for (uint8_t i = 0; i<16; i++) {  
    int pixels[1] = {i};
    fadeIn(pixels, 1, -5);
  } 
  
  for (uint8_t i = 1; i<16; i++) {  
    int pixels[1] = {i};
    fadeOut(pixels, 1, -5);
  }
  
  minutes = 1;
  displayMinutes();
}

void displayMinutes() {
  same_color_index = 2;
  for (uint8_t i = 0; i<constrain(minutes-1, 0, 16); i++) {  
    strip.setPixelColor(i, strip.Color(colors[same_color_index][0], colors[same_color_index][1], colors[same_color_index][2]));
  }
  
  int activePixel = constrain((minutes-1), 0, 15);
  
  // blink active until interrupted
  interrupted = false;
  activate_exit = true;
  int blink_pixels[1] = {activePixel};
  while (!interrupted) {
     fadeOut(blink_pixels, 1, 2);
     delayWithInput(30);
     fadeIn(blink_pixels, 1, 1);
     delayWithInput(180); 
   }
   
   off();
   same_color_index = -1;
   countdown_mode = false;
}

void onCountdownShortButtonPress() {
  if (countdown_mode) {
    addMinute();
  }
}

void onCountdownLongButtonPress() {
  if (activate_exit) {
    activate_exit = false;
    interrupted = true;
  }
}

void addMinute() {
  minutes = constrain(minutes+1, 1, 16);
  displayMinutes();
}
