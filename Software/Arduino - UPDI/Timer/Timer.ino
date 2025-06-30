#include "RTClib.h"
RTC_DS3231 rtc;

const int pin_count = 16;
const int pins[pin_count] = {4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};  
const int LED_count = 201;
int LOW_POS[] = {0};
int HIGH_POS[] = {4};

DateTime start;

void setup() {
  rtc.begin();
  start = rtc.now();
}

void loop() {
  DateTime now = rtc.now();

  // // Test using 3-second delay (change to 3600 for 1 hour)
  if ((now.second() - start.second()) < 2 || (now.hour() - start.hour()) > 7) {
    for (int i = 0; i < 1; i++) {
      pinMode(pins[LOW_POS[i]], OUTPUT);
      pinMode(pins[HIGH_POS[i]], OUTPUT);

      digitalWrite(pins[LOW_POS[i]], LOW);
      digitalWrite(pins[HIGH_POS[i]], HIGH);

      delayMicroseconds(100);

      pinMode(pins[LOW_POS[i]], INPUT);
      pinMode(pins[HIGH_POS[i]], INPUT);
    }
    // Reset the timer
    // start = now;
  }
  else{
    delay(500);
  }
}
