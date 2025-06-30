#include "RTClib.h"
RTC_DS3231 rtc;

const int pin_count = 16;
const int pins[pin_count] = {4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
const int Button = 22;

const int LED_count = 201;
int LOW_POS[] = {0};
int HIGH_POS[] = {4};

DateTime start;

void setup() {
  rtc.begin();

  if (rtc.lostPower()) {
    // Sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  start = rtc.now();
  pinMode(Button, INPUT);
}

void loop() {
  DateTime now = rtc.now();

  // Use unixtime to correctly track elapsed time across seconds/minutes
  if ((now.unixtime() - start.unixtime()) <= 2 || (now.unixtime() - start.unixtime()) >= 10+2  ) {
  // if ((now.unixtime() - start.unixtime()) >= 1) {

    for (int i = 0; i < 1; i++) {
      pinMode(pins[LOW_POS[i]], OUTPUT);
      pinMode(pins[HIGH_POS[i]], OUTPUT);

      digitalWrite(pins[LOW_POS[i]], LOW);
      digitalWrite(pins[HIGH_POS[i]], HIGH);

      delayMicroseconds(100);

      pinMode(pins[LOW_POS[i]], INPUT);
      pinMode(pins[HIGH_POS[i]], INPUT);
    }
  } 
  // else {
  //   delay(10);
  // }

  // Reset timer if button is pressed
  if (digitalRead(Button) == HIGH) {
    start = now;
  }
}
