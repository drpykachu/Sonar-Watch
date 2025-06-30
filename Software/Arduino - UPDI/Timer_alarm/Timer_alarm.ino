#include "RTClib.h"
RTC_DS3231 rtc;
#pragma message "Compiled on: " __DATE__ " at " __TIME__

const int pin_count = 16;
const int pins[pin_count] = {4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};

const int LED_count = 201;
int LOW_POS[] = {0};
int HIGH_POS[] = {4};

const int trigger_hour = 22;   // 10 PM
const int trigger_minute = 45; // 26 minutes
bool triggered = false;

void setup() {
  rtc.begin();
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop() {
  DateTime now = rtc.now();

  if (!triggered && now.hour() == trigger_hour && now.minute() == trigger_minute-1) {
    // Trigger once at 10:26 PM
    for (int i = 0; i < 1; i++) {
      pinMode(pins[LOW_POS[i]], OUTPUT);
      pinMode(pins[HIGH_POS[i]], OUTPUT);

      digitalWrite(pins[LOW_POS[i]], LOW);
      digitalWrite(pins[HIGH_POS[i]], HIGH);
    }
    triggered = true; // Don't trigger again
  }
}
