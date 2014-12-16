#define BUZZERPIN 3

unsigned long start_time = 0;
unsigned long end_time = 0;

void setup() {
  Serial.begin(9600);
  pinMode(BUZZERPIN, OUTPUT);
  
  start_time = millis();
}

void loop() {
  delay(1000-1);
  digitalWrite(BUZZERPIN, HIGH);
  delay(1);
  digitalWrite(BUZZERPIN, LOW);
  
  unsigned long now = millis();
  unsigned long elapsed = now - start_time;
  
  Serial.print("end_time: ");
  Serial.print(end_time);
  Serial.print("now: ");
  Serial.print(now);
  Serial.print(" elapsed: ");
  Serial.println(elapsed);
  
  if (end_time < now || end_time == 0) {
     start_time = now;
     end_time = start_time + 5000;
     buzz();
  } 
}

void buzz() {
    for (int i=0; i<10; i++) {
      digitalWrite(BUZZERPIN, HIGH);
      delay(2);
      digitalWrite(BUZZERPIN, LOW);
      delay(2);
    }
}
