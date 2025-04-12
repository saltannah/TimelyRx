const int switchPin = 2;
bool boxOpen = false;

void setup() {
  pinMode(switchPin, INPUT_PULLUP);  // Uses internal pull-up resistor
  Serial.begin(9600);
}

void loop() {
  int state = digitalRead(switchPin);
  if (state == LOW) {
    if (!boxOpen) {
      boxOpen = true;
      Serial.println("Box opened!");
      // Call your dose logging function here
    }
  } else {
    boxOpen = false;  // Reset state when switch is closed again
  }

  delay(50);  // Simple debounce
}

