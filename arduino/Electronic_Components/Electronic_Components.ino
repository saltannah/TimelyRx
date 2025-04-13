#include "RTC.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSans9pt7b.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

const int toggle = 2;
bool boxOpen = false;
bool missedDose = false;
String lastDose;
int prevState = HIGH;
int doseHourly = 8;
unsigned long timerStart;
unsigned long doseInc = doseHourly * 60 * 60 * 1000; // hour to milliseconds
unsigned long nextDose;
unsigned long lastUpdate = 0;
const unsigned long updateInterval = 1000; // for refresh rate of screen

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Wire.begin();
  pinMode(toggle, INPUT_PULLUP);  // Uses internal pull-up resistor
  Serial.begin(9600);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // Initialize screen with I2C address
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  //display.println("Thank you for using \n TimelyRx!");
  display.display();

  RTC.begin();

  timerStart = millis();

  //DELETE AFTER USE - only used to set time when arduino is started
  //RTCTime startTime(13, Month::APRIL, 2025, 06, 19, 30, DayOfWeek::WEDNESDAY, SaveLight::SAVING_TIME_ACTIVE);
  //RTC.setTime(startTime);
}

void loop() {
  unsigned long currentMillis = millis();
  int currentState = digitalRead(toggle);

  nextDose = doseInc - (currentMillis - timerStart);
  
  // Check if state changed
  if (currentState != prevState) {
    delay(50);  // debounce delay
    currentState = digitalRead(toggle); // re-read after debounce

    if (currentState != prevState) { // double check
      prevState = currentState; //set previous state

      if (currentState == LOW) { //circuit is closed
        boxOpen = true;
        timerStart = millis();
        lastDose = getDoseTime();
        missedDose = false;
        sendTimeStamp();
        
      } else {
        boxOpen = false;
      }
    }
  }
  if (currentMillis - lastUpdate >= updateInterval) {
    displayDoseTime(lastDose);
    lastUpdate = currentMillis;
  }

  if (nextDose <= 0){
    nextDose = doseInc;
    missedDose = true;
  }
}

String getDoseTime(){
  RTCTime currentTime;
  RTC.getTime(currentTime);
  int hour = currentTime.getHour();
  int min = currentTime.getMinutes();
  const char* timeOfDay = "AM";

  if (hour >= 12) {
    timeOfDay = "PM";
    if (hour > 12) {
      hour -= 12;
    }
  } else if (hour == 0) {
    hour = 12;  // midnight edge case
  }

  char buffer[10];
  sprintf(buffer, "%02d:%02d %s", hour, min, timeOfDay);

  return String(buffer);
}

String calcNextDose(){
  int nextHour = nextDose / (60 * 60 * 1000);
  int nextMin = (nextDose % (60 * 60 * 1000)) / (60 * 1000);
  int nextSec = (nextDose % (60 * 1000)) / 1000;

  char buffer[11];
  sprintf(buffer, "%d:%02d:%02d", nextHour, nextMin, nextSec);  // countdowm timer format
  return String(buffer);
}

void displayDoseTime(String dosageTime){ //add in the skip dose feature!!
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.setTextSize(1);

  if(nextDose < doseInc / 2 && missedDose){
    display.println("Dose is overdue :(");
    display.println("");
    display.println("Skip this one!");
  }

  else{
    display.print("Last Dose: ");
    display.println(dosageTime);
    display.println("");
  }

  display.println("");
  display.println("Next Dose In:");
  display.println("");
  display.setTextSize(2);
  display.print(calcNextDose());

  display.display();// Push buffer to screen
}

void sendTimeStamp(){
  RTCTime currentTime;
  RTC.getTime(currentTime);

  char buffer[20];
  sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d", //YYYY-MM-DD HH:MM:SS format
        currentTime.getYear(),
        static_cast<int>(currentTime.getMonth()),
        currentTime.getDayOfMonth(),
        currentTime.getHour(),
        currentTime.getMinutes(),
        currentTime.getSeconds());

  Serial.println(buffer);  // Send this over to Python
}
