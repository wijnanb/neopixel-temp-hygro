
int minutes;
bool interrupted = false;
bool activate_exit = false;
bool ripple_count;
bool alarm_interrupted = false;
bool alarm_ringing = false;
int activePixel;

unsigned long end_time;
unsigned long last_tick = 0;

void enterCountdownMode() {
  countdown_mode = true;
  alarm_ringing = false;
  same_color_index = random(16);
  
  ripple();
  
  minutes = 0;
  addMinute();
  
  startAnimation();
}

void startAnimation() {
  do {  
     int blink_pixels[1] = {activePixel};
   
     fadeOut(blink_pixels, 1, 2);
     delayWithInput(30);
     fadeIn(blink_pixels, 1, 1);
     delayWithInput(180);
     
     checkTime();
  } while (!interrupted);
    
  same_color_index = -1;
  countdown_mode = false;
  off();
}

void onCountdownShortButtonPress() {
  if (alarm_ringing) {
    alarm_interrupted = true;
  } else {
    addMinute();
  }
}

void onCountdownLongButtonPress() {
  if (activate_exit) {
    activate_exit = false;
    interrupted = true;
  }
}

unsigned long getMillis() {
   return (unsigned long) millis(); 
}

void addMinute() {
  minutes = constrain(minutes+1, 1, 16);
  end_time = millis() + ((unsigned long)minutes)*10*1000;
  
  updateMinutes();
}

void updateMinutes() {
  for (uint8_t i = 0; i<16; i++) {
    if (i<constrain(minutes, 0, 16)) {
      strip.setPixelColor(i, strip.Color(colors[same_color_index][0], colors[same_color_index][1], colors[same_color_index][2]));
    } else {
      strip.setPixelColor(i, black);
    }
  }
  
  // blink active until interrupted
  interrupted = false;
  activate_exit = true;
  
  activePixel = constrain((minutes-1), 0, 15);
}

void checkTime() {
  unsigned long now = millis();
  unsigned long time_left = end_time - now; // will overflow, cannot be negative
  
  Serial.print("end_time: ");
  Serial.print(end_time);
  Serial.print("now: ");
  Serial.print(now);
  Serial.print(" time_left: ");
  Serial.println(time_left);
  
  if (end_time != 0) {
     if (end_time <= now) {
        end_time = 0;
        interrupted = true;
        alarm();        
     } else {
       int new_minutes = ceil((time_left)/(10*1000))+1;
       
       if (new_minutes < minutes) {
         minutes = new_minutes;
         updateMinutes();
         
         Serial.println(minutes);
         tick3();
       } else {
         if (last_tick == 0 || (last_tick + 1000) < now) {
            last_tick = now;
            tick(); 
         }
       }
     }
  }
}

void alarm() {
  alarm_ringing = true;
  alarm_interrupted = false;
  for (int i=0; i<10; i++) {
    if (!alarm_interrupted) {
      buzz();
      ripple();
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
