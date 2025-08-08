// WatchData.h

#ifndef Module_WristMode_H
#define Module_WristMode_H

//////////////////////////////////////////////////////////////// Used External Variables
extern RTC_DS3231 rtc;
extern DateTime now;
extern int hour;
extern int minute;
extern int clock_mode;
extern int brightness;
extern bool options_section;
extern const int Button;
extern unsigned long fade_duration;

//////////////////////////////////////////////////////////////// Module Functions

int WristMode_Timer(int threshold_ms) {
  static bool waiting_for_release = false;
  static unsigned long press_time = 0;

  // checks the status of the button and when it was pressed
  if (digitalRead(Button) == HIGH) {
    // Button pressed now
    if (waiting_for_release == false) {
      press_time = millis();
      waiting_for_release = true;
    }
    // Check how long it's been held
    if (millis() - press_time > threshold_ms) {
      waiting_for_release = false;
      return false;  // Long hold → enter options menu
    }
  }

  else if (waiting_for_release) {
    // Button was just released
    waiting_for_release = false;
    if (millis() - press_time <= threshold_ms) {
      wrist_mode = wrist_mode + 1;
      if (wrist_mode == 2){
        wrist_mode = 0;
      }
      
    }
  }
  return true;
}

//////////////////////////////////////////////////////////////// Module Core
void Mod_WristMode() {
    while (options_section == true) {
      options_section = WristMode_Timer(1000);
      for (uint16_t colCnt = 0; colCnt < LEFT_RIGHT_matrix[wrist_mode].numElements; colCnt++) {
        LED_ON(LEFT_RIGHT_matrix[wrist_mode].pins[colCnt], brightness, 100, false);
      }
    }
}
#endif