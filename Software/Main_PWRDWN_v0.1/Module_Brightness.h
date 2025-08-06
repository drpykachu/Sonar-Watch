// WatchData.h

#ifndef Module_Brightness_H
#define Module_Brightness_H

//////////////////////////////////////////////////////////////// Used External Variables

extern int brightness;
extern bool options_section;
extern const int Button;

//////////////////////////////////////////////////////////////// Module Functions

int Brightness_Timer(int threshold_ms) {
  static bool waiting_for_release = false;
  static unsigned long press_time = 0;
  // static first_press = true;
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
      return false;  // Long hold → enter options menu
    }
  } 
  
  else if (waiting_for_release) {
    // Button was just released
    waiting_for_release = false;
    if (millis() - press_time <= threshold_ms) {
      brightness += 10;
      if (brightness > 100) {
        brightness = 10;
      }
    }
  }
  return true;
}

//////////////////////////////////////////////////////////////// Module Core

void Mod_Brightness(){
  while (options_section == true) {
    options_section = Brightness_Timer(1000);
    for (uint16_t colCnt = 0; colCnt < large_seg7_matrix[(brightness - 10) / 10].numElements; colCnt++) {
      LED_ON(large_seg7_matrix[(brightness - 10) / 10].pins[colCnt], brightness, 100, false);
    }
  }
}




#endif