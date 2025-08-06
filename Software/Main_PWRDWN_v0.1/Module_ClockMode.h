// WatchData.h

#ifndef Module_ClockMode_H
#define Module_ClockMode_H

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


int ClockMode_Timer(int threshold_ms) {
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
      clock_mode = clock_mode + 1;
      if (clock_mode == 2){
        clock_mode = 0;
      }
      
    }
  }
  return true;
}

void LED_Fade_Clock(int hour, int minute, int duration, int pre_LED_count, int brightness) {
  int hour_dig_r = hour % 10;
  int hour_dig_l = (hour - hour_dig_r) / 10;
  int min_dig_r = minute % 10;
  int min_dig_l = (minute - minute % 10) / 10;
  unsigned long fade_start = millis();
  
  while (millis() - fade_start < duration) {
      // Calculate how far along we are (0.0 to 1.0)
      float progress = (millis() - fade_start) / (float) duration;

      int brightness_fade = brightness - (int)(progress * brightness);

      for (uint16_t colCnt = 0; colCnt < pre_LED_count; colCnt++) {
        LED_ON(0, brightness_fade, 100,true);
      }

    for (uint16_t colCnt = 0; colCnt < small_seg7_matrix[min_dig_l].numElements; colCnt++) {
      LED_ON(small_seg7_matrix[min_dig_l].pins[colCnt]+2, brightness_fade, 100, false);
    }
    for (uint16_t colCnt = 0; colCnt < small_seg7_matrix[min_dig_r].numElements; colCnt++) {
      LED_ON(small_seg7_matrix[min_dig_r].pins[colCnt]+6, brightness_fade, 100, false);
    }
    for (uint16_t colCnt = 0; colCnt < small_seg7_matrix[hour_dig_l].numElements; colCnt++) {
      LED_ON(small_seg7_matrix[hour_dig_l].pins[colCnt] - 6, brightness_fade, 100, false);
    }
    for (uint16_t colCnt = 0; colCnt < small_seg7_matrix[hour_dig_r].numElements; colCnt++) {
      LED_ON(small_seg7_matrix[hour_dig_r].pins[colCnt] - 2, brightness_fade, 100, false);
    }

  }
}  


void Clock_Execute(int hour, int minute) {
  int hour_dig_r = hour % 10;
  int hour_dig_l = (hour - hour_dig_r) / 10;
  int min_dig_r = minute % 10;
  int min_dig_l = (minute - minute % 10) / 10;
  int display_count = (small_seg7_matrix[hour_dig_l].numElements + small_seg7_matrix[hour_dig_r].numElements + small_seg7_matrix[min_dig_l].numElements + small_seg7_matrix[min_dig_r].numElements);
  for (int waiter = 0; waiter < 5000 / display_count; waiter++) {
    for (uint16_t colCnt = 0; colCnt < small_seg7_matrix[min_dig_l].numElements; colCnt++) {
      LED_ON(small_seg7_matrix[min_dig_l].pins[colCnt]+2, brightness, 100, false);
    }
    for (uint16_t colCnt = 0; colCnt < small_seg7_matrix[min_dig_r].numElements; colCnt++) {
      LED_ON(small_seg7_matrix[min_dig_r].pins[colCnt]+6, brightness, 100, false);
    }
    for (uint16_t colCnt = 0; colCnt < small_seg7_matrix[hour_dig_l].numElements; colCnt++) {
      LED_ON(small_seg7_matrix[hour_dig_l].pins[colCnt] - 6, brightness, 100, false);
    }
    for (uint16_t colCnt = 0; colCnt < small_seg7_matrix[hour_dig_r].numElements; colCnt++) {
      LED_ON(small_seg7_matrix[hour_dig_r].pins[colCnt] - 2, brightness, 100, false);
    }
  }
  LED_Fade_Clock(hour, minute, fade_duration, 1, brightness);

}

void LED_Fade_Ring(int hour_LED, int minute_LED, int duration, int pre_LED_count, int brightness) {
  
  unsigned long fade_start = millis();
  
  while (millis() - fade_start < duration) {
      // Calculate how far along we are (0.0 to 1.0)
      float progress = (millis() - fade_start) / (float) duration;

      int brightness_fade = brightness - (int)(progress * brightness);

      for (uint16_t colCnt = 0; colCnt < pre_LED_count; colCnt++) {
        LED_ON(0, brightness_fade, 100,true);
      }

      LED_ON(hour_ring[hour_LED], brightness_fade,1000, false);
      LED_ON(minute_ring[minute_LED], brightness_fade,1000, false);

  }
}  


void Ring_Execute(int hour_LED, int minute_LED) {
  bool hour_switch = false;
  bool minute_switch = false;
  for (uint16_t rowCnt = 0; rowCnt < 12; rowCnt++) {
    for (int waiter = 0; waiter < 300 / ringMatrix[rowCnt].numElements; waiter++) {
      for (uint16_t colCnt = 0; colCnt < ringMatrix[rowCnt].numElements; colCnt++) {
        LED_ON(ringMatrix[rowCnt].pins[colCnt], brightness, 100, false);


        if (ringMatrix[rowCnt].pins[colCnt] == hour_ring[hour_LED]) {
          hour_switch = true;
        }
        if (ringMatrix[rowCnt].pins[colCnt] == minute_ring[hour_LED]) {
          minute_switch = true;
        }
      }

      if (hour_switch == true) {
        LED_ON(hour_ring[hour_LED], brightness, 1000, false);
      }
      if (minute_switch == true) {
        LED_ON(minute_ring[minute_LED], brightness, 1000, false);
      }
    }
  }
  LED_Fade_Ring(hour_LED, minute_LED, fade_duration, ringMatrix[11].numElements, brightness);
}


//////////////////////////////////////////////////////////////// Module Core

void Mod_ClockMode() {

  while (options_section == true) {
    options_section = ClockMode_Timer(1000);
    for (uint16_t colCnt = 0; colCnt < PING4DIG_matrix[clock_mode].numElements; colCnt++) {
      LED_ON(PING4DIG_matrix[clock_mode].pins[colCnt], brightness, 100, false);
    }
  }
}

#endif