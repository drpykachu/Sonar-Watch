// WatchData.h

#ifndef Module_Hour_H
#define Module_Hour_H

//////////////////////////////////////////////////////////////// Used External Variables
extern RTC_DS3231 rtc;
extern DateTime now;
extern int hour;
extern int minute;

extern int brightness;
extern bool options_section;
extern const int Button;

//////////////////////////////////////////////////////////////// Module Functions

int Hour_Timer(int threshold_ms) {
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
      waiting_for_release = false; // Reset for next time
      now = rtc.now();
      rtc.adjust(DateTime(now.year(), now.month(), now.day(), hour, now.minute(), now.second()));  // year, month, day, hour, min, sec
      
      return false;  // Long hold → enter options menu
    }
  } 
  
  else if (waiting_for_release) {
    // Button was just released
    waiting_for_release = false;
    if (millis() - press_time <= threshold_ms) {
      hour += 1;
      if (hour > 12) {
        hour = hour - 12;
      }
    }
  }
  return true;
}


//////////////////////////////////////////////////////////////// Module Core
void Mod_Hour(){
    unsigned long hour_flash_timer = millis();  // milliseconds
    int flash_switch = true;      
    while (options_section == true) {
      options_section = Hour_Timer(1000);
      int hour_dig_r = hour % 10 ;
      int hour_dig_l = (hour - hour_dig_r)/10;
      int min_dig_r = minute % 10 ;
      int min_dig_l = (minute - minute % 10)/10;
      if(millis() - hour_flash_timer > 375 && flash_switch == false){
        flash_switch = true;
        hour_flash_timer = millis();

        }
      if(millis() - hour_flash_timer > 375 && flash_switch == true){
        flash_switch = false;
        hour_flash_timer = millis();}

        if(flash_switch == true){
          for (uint16_t colCnt = 0; colCnt < small_seg7_matrix[hour_dig_l].numElements; colCnt++) {
            LED_ON(small_seg7_matrix[hour_dig_l].pins[colCnt]-6, brightness, 100, false);
          }
          for (uint16_t colCnt = 0; colCnt < small_seg7_matrix[hour_dig_r].numElements; colCnt++) {
            LED_ON(small_seg7_matrix[hour_dig_r].pins[colCnt]-2, brightness, 100, false);
          }
        }
        else{
          for (uint16_t colCnt = 0; colCnt < small_seg7_matrix[hour_dig_l].numElements; colCnt++) {
            LED_ON(small_seg7_matrix[hour_dig_l].pins[colCnt]-6, brightness, 100, true);
          }
          for (uint16_t colCnt = 0; colCnt < small_seg7_matrix[hour_dig_r].numElements; colCnt++) {
            LED_ON(small_seg7_matrix[hour_dig_r].pins[colCnt]-2, brightness, 100, true);
          }

        }
        for (uint16_t colCnt = 0; colCnt < small_seg7_matrix[min_dig_l].numElements; colCnt++) {
          LED_ON(small_seg7_matrix[min_dig_l].pins[colCnt]+2, brightness, 100, false);
        }
        for (uint16_t colCnt = 0; colCnt < small_seg7_matrix[min_dig_r].numElements; colCnt++) {
          LED_ON(small_seg7_matrix[min_dig_r].pins[colCnt]+6, brightness, 100, false);
        }

    }
}

#endif