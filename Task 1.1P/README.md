// Task1.1Lights_ON

const int toggle = 12;
const int porch = 11; // ON for 30sec
const int hall = 10;  // ON for 60sec

// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(toggle, INPUT);
  pinMode(porch, OUTPUT);
  pinMode(hall, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  if (digitalRead(toggle) == HIGH) { 
    turnOn(porch);
    turnOn(hall);

    turnOff(porch);  
    turnOff(hall);  

    delay(3000);  // delay 3sec to observe 
  }
}

void turnOn(int LED) {
  digitalWrite(LED, HIGH);
}

void turnOff(int LED) {
  delay(30000); // delay 30s
  digitalWrite(LED, LOW);
}
