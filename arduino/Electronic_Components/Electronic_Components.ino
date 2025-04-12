#include "RTC.h"

const int toggle = 2;
bool boxOpen = false;
int prevState = HIGH;

void setup() {
  pinMode(toggle, INPUT_PULLUP);  // Uses internal pull-up resistor
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // Initialize screen with I2C address

  RTC.begin();

  //DELETE AFTER USE - only used to set time when arduino is started
  RTCTime startTime(12, Month::APRIL, 2025, 01, 46, 00, DayOfWeek::WEDNESDAY, SaveLight::SAVING_TIME_ACTIVE);
  RTC.setTime(startTime);
}

void loop() {

  int currentState = digitalRead(toggle);

  // Check if state changed
  if (currentState != prevState) {
    delay(50);  // debounce delay
    currentState = digitalRead(toggle); // re-read after debounce

    if (currentState != prevState) { // double check
      prevState = currentState; //set previous state

      if (currentState == LOW) {
        boxOpen = true;
        Serial.println("Box opened, data logged!");
        displayTime(getDoseTime());
        
      } else {
        boxOpen = false;
      }
    }
  }
}

String getDoseTime(){
  RTCTime currentTime;
  RTC.getTime(currentTime);

  char buffer[6];
  sprintf(buffer, "%02d:%02d", currentTime.getHour(), currentTime.getMinutes());

  return String(buffer);
}

void displayTime(String dosageTime){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  display.println("Dose logged at:");
  display.println(dosageTime);

  display.display();// Push buffer to screen
}

void sendTimeStamp(){
  RTCTime currentTime;
  RTC.getTime(currentTime);

  char buffer[20];
  sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d", //YYYY-MM-DD HH:MM:SS format
        currentTime.getYear(),
        static_cast<int>(currentTime.getMonth()),
        currentTime.getDay(),
        currentTime.getHour(),
        currentTime.getMinutes(),
        currentTime.getSeconds());

Serial.println(buffer);  // Send this over to Python

}
