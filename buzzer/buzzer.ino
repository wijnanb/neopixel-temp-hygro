#define BUZZERPIN 3

void setup() {
  Serial.begin(9600);
  pinMode(BUZZERPIN, OUTPUT);
}

void loop() {
  buzz();
  delay(2000);
}

void buzz() {
  for (int j=0; j<3; j++) {
    for (int i=0; i<10; i++) {
      digitalWrite(BUZZERPIN, HIGH);
      delay(2);
      digitalWrite(BUZZERPIN, LOW);
      delay(2);
    }
    
    delay(50);
  }
}
