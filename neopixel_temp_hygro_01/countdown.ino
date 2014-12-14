
int minutes;
bool interrupted = false;
bool activate_exit = false;
bool ripple_count;
long end_time;
int activePixel;

void enterCountdownMode() {
  countdown_mode = true;
  same_color_index = random(16);
  
  ripple();
  
  minutes = 0;
  addMinute();
  
  startTimer();
}

void startTimer() {
  do {  
     int blink_pixels[1] = {activePixel};
   
     fadeOut(blink_pixels, 1, 2);
     delayWithInput(30);
     fadeIn(blink_pixels, 1, 1);
     delayWithInput(180);
     
     //checkTime();
  } while (!interrupted);
    
  same_color_index = -1;
  countdown_mode = false;
  off();
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
  end_time = millis() + minutes*3*1000;
  
  for (uint8_t i = 0; i<constrain(minutes, 0, 16); i++) {  
    strip.setPixelColor(i, strip.Color(colors[same_color_index][0], colors[same_color_index][1], colors[same_color_index][2]));
  }
  
  // blink active until interrupted
  interrupted = false;
  activate_exit = true;
  
  activePixel = constrain((minutes-1), 0, 15);
}

void checkTime() {
  if (end_time != 0) {
     if (end_time < millis()) {
        // alarm!
        end_time = 0;
        interrupted = true;
        ripple();
     } else {
       minutes = ceil((end_time - millis())/3*1000);
     }
  }
}

void ripple() {
  int d = -25;
  int passes = 5;
  int max_length = 10;
  
  int pixels[1] = {3};
  fadeIn(0, 1, d);
  
  int l = 1;
  int pos = 3;
  bool grow = true;
  bool shrink = false;
  for (int i=0; i<61; i++) {    
     if (grow) {
       int grow_pixels[1] = {(pos + l) % 16}; 
       fadeIn(grow_pixels, 1, d);
       l++;
     } else {
       int move_pixels[1] = {pos % 16}; 
       fadeOut(move_pixels, 1, d);
       
       int grow_pixels[1] = {(pos + l) %16}; 
       fadeIn(grow_pixels, 1, d);
       pos++;
     }
     
     if (i%7 == 0 && i!= 0) {
       grow = true;
     } else {
       grow = false;
     }
  }
  
  
  for (int i=0; i<9; i++) {    
    int out_pixels[1] = {7 + i}; 
    fadeOut(out_pixels, 1, floor(d/2));
  }
  
  off();
}
