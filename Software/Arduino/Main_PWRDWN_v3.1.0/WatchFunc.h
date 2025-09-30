// WatchFunc.h

#ifndef WatchFunc_H
#define WatchFunc_H

extern const int Button;
extern int LED_flip;
extern int wrist_mode;
extern const int Bat_READ;
extern const int Bat_ON;
extern int brightness;
extern const int Charge_STAT;
extern DateTime now;
extern RTC_DS3231 rtc;
extern int hour;
extern int minute;
extern int clock_mode;
extern unsigned long fade_duration;
extern bool options_section;
extern bool options_mode;

//////////////////////////////////////////////////////////////// LED CONTROL FUNCTION
void LED_ON(int LED, int brightness, int timer, bool dummy) {
  unsigned long total_cycle = timer;

  // Clamp and scale brightness
  brightness = constrain(brightness, 0, 100);
  if (brightness == 0) {
    delayMicroseconds(total_cycle);
    return;
  }

  float scale = brightness / 100.0;
  scale *= scale;  // perceptual brightness
  int on_time = (int)(total_cycle * scale);
  int off_time = total_cycle - on_time;

  // Index correction
  LED = LED - 1;
  int LED_input;
  if (wrist_mode == 1) {
    LED_input = 200 - LED;
  } else {
    LED_input = LED;
  }

  int low_pin_index = LOW_POS[LED_input];
  int high_pin_index = HIGH_POS[LED_input];

  int low_pin = pins[low_pin_index];
  int high_pin = pins[high_pin_index];

  // Set LOW and HIGH
  pinMode(low_pin, OUTPUT);
  pinMode(high_pin, OUTPUT);

  digitalWrite(low_pin, LOW);
  if (dummy == false) {
    digitalWrite(high_pin, HIGH);
  } else {
    digitalWrite(high_pin, LOW);
  }

  delayMicroseconds(on_time);

  // WTF THIS RESETS IT ALL
  digitalWrite(high_pin, LOW);

  // Reset pins
  pinMode(low_pin, INPUT);
  pinMode(high_pin, INPUT);
  delayMicroseconds(off_time);
}

//////////////////////////////////////////////////////////////// Flash ALL
void CHG_flash(int repeat) {
  unsigned long hold_timer = millis();
  for (int i = 0; i < repeat; i++) { 
    for (int waiter = 0; waiter < 2500 / CHG_matrix[0].numElements; waiter++) {
      for (uint16_t colCnt = 0; colCnt < CHG_matrix[0].numElements; colCnt++) {
        LED_ON(CHG_matrix[0].pins[colCnt], brightness, 100, false);
      }
    }   
    delay(200); 
  }
}

//////////////////////////////////////////////////////////////// Flash ALL
void All_flash(float duration, int brightness, int repeat) {

  for (int i = 0; i < repeat; i++) {
    unsigned long flash_start = millis();
    while (millis() - flash_start < duration * 1000) {

      // PARROT
      // const int ROWER[] = {39, 40, 41, 42, 43, 53, 59, 67, 69, 73, 75, 82, 85, 86, 87, 91, 97, 99, 101, 103, 106, 112, 114, 118, 121, 127, 130, 132, 136, 141, 146, 152, 155, 167, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181};      RINGS ROWER_RING[] = 
      // {  {ROWER, NUMELEMENTS(ROWER)}};
      // for (uint16_t rowCnt = 0; rowCnt < NUMELEMENTS(ROWER_RING); rowCnt++) {
      //   for (int waiter = 0; waiter < 1000 / ROWER_RING[rowCnt].numElements; waiter++) {
      //     for (uint16_t colCnt = 0; colCnt < ROWER_RING[rowCnt].numElements; colCnt++) {
      //       LED_ON(ROWER_RING[rowCnt].pins[colCnt], 100, 100, false);
      //     }
      //   }
      // }

      // BLANK
      for (int i = 0; i < LED_count; i++) {
        LED_ON(i, brightness, 50, false);
      }
    }


    delay(100);
  }
}

//////////////////////////////////////////////////////////////// Hold Timer - for Options Mode
bool Hold_Timer(int milliseconds) {
  unsigned long hold_timer = millis();
  while (digitalRead(Button) == true) {
    if (millis() - hold_timer > milliseconds) {  // time for options menu criteria
      return true;
      break;
    }
  }
  return false;
}

//////////////////////////////////////////////////////////////// Crab Timer
void Crab_Timer(int milliseconds) {
  unsigned long hold_timer = millis();
  while (digitalRead(Button) == true) {
    if (millis() - hold_timer > milliseconds) {  // time for options menu criteria
      for (uint16_t rowCnt = 0; rowCnt < 8; rowCnt++) {
        for (int waiter = 0; waiter < 1500 / CRAB_matrix[rowCnt].numElements; waiter++) {
          for (uint16_t colCnt = 0; colCnt < CRAB_matrix[rowCnt].numElements; colCnt++) {
            LED_ON(CRAB_matrix[rowCnt].pins[colCnt], 100, 100, false);
          }
        }
      }
    }
  }
}


/////////////////////////////////////////////////////////////////// Charge Indicator
void Mod_Charge() {
  if (digitalRead(Charge_STAT) == LOW) {
    LED_ON(1, 5, 1000, false);
  }
}

/////////////////////////////////////////////////////////////////// LED 4-Digit Display
void LED_Fade_Clock(int hour, int minute, int duration, int pre_LED_count, int brightness) {
  int hour_dig_r = hour % 10;
  int hour_dig_l = (hour - hour_dig_r) / 10;
  int min_dig_r = minute % 10;
  int min_dig_l = (minute - minute % 10) / 10;
  unsigned long fade_start = millis();

  while (millis() - fade_start < duration) {
    // Calculate how far along we are (0.0 to 1.0)
    float progress = (millis() - fade_start) / (float)duration;

    int brightness_fade = brightness - (int)(progress * brightness);
    Mod_Charge();

    for (uint16_t colCnt = 0; colCnt < pre_LED_count; colCnt++) {
      LED_ON(0, brightness_fade, 100, true);
    }

    for (uint16_t colCnt = 0; colCnt < small_seg7_matrix[min_dig_l].numElements; colCnt++) {
      LED_ON(small_seg7_matrix[min_dig_l].pins[colCnt] + 2, brightness_fade, 100, false);
    }
    for (uint16_t colCnt = 0; colCnt < small_seg7_matrix[min_dig_r].numElements; colCnt++) {
      LED_ON(small_seg7_matrix[min_dig_r].pins[colCnt] + 6, brightness_fade, 100, false);
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
    Mod_Charge();
    for (uint16_t colCnt = 0; colCnt < small_seg7_matrix[min_dig_l].numElements; colCnt++) {
      LED_ON(small_seg7_matrix[min_dig_l].pins[colCnt] + 2, brightness, 100, false);
    }
    for (uint16_t colCnt = 0; colCnt < small_seg7_matrix[min_dig_r].numElements; colCnt++) {
      LED_ON(small_seg7_matrix[min_dig_r].pins[colCnt] + 6, brightness, 100, false);
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

/////////////////////////////////////////////////////////////////// LED Ring Display
void LED_Fade_Ring(int hour_LED, int minute_LED, int duration, int pre_LED_count, int brightness) {

  unsigned long fade_start = millis();

  while (millis() - fade_start < duration) {

    // Calculate how far along we are (0.0 to 1.0)
    float progress = (millis() - fade_start) / (float)duration;

    int brightness_fade = brightness - (int)(progress * brightness);

    for (uint16_t colCnt = 0; colCnt < pre_LED_count; colCnt++) {
      LED_ON(0, brightness_fade, 100, true);
    }

    LED_ON(hour_ring[hour_LED], brightness_fade, 1000, false);
    LED_ON(minute_ring[minute_LED], brightness_fade, 1000, false);
  }
}
void Ring_Execute(int hour_LED, int minute_LED) {
  bool hour_switch = false;
  bool minute_switch = false;
  for (uint16_t rowCnt = 0; rowCnt < 12; rowCnt++) {
    Mod_Charge();
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

/////////////////////////////////////////////////////////////////// Module timer - All
int Module_Timer(int threshold_ms, const char* module_type) {
  static bool waiting_for_release = false;
  static unsigned long press_time = millis();
  static bool start_flag = false;
  int over_time_ms = 3000;

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

      waiting_for_release = false;  // Reset for next time
      start_flag = false;

      if (strcmp(module_type, "Hour") == 0) {
        now = rtc.now();                                                                             // Reset for next time
        rtc.adjust(DateTime(now.year(), now.month(), now.day(), hour, now.minute(), now.second()));  // year, month, day, hour, min, sec
      }

      if (strcmp(module_type, "Minute") == 0) {
        now = rtc.now();                                                                             // Reset for next time
        rtc.adjust(DateTime(now.year(), now.month(), now.day(), now.hour(), minute, now.second()));  // year, month, day, hour, min, sec
      }

      return false;  // Long hold → enter options menu
    }
  }

  else if (waiting_for_release) {
    // Button was just released
    waiting_for_release = false;

    if (start_flag == false) {
      start_flag = true;

    }

    else {
      if (millis() - press_time <= threshold_ms) {

        //////// brightness module
        if (strcmp(module_type, "Brightness") == 0) {
          brightness += 10;
          if (brightness > 100) {
            brightness = 10;
          }
        }
        //////// Hour module
        if (strcmp(module_type, "Hour") == 0) {
          hour += 1;
          if (hour > 12) {
            hour = hour - 12;
          }
        }
        //////// Minute module
        if (strcmp(module_type, "Minute") == 0) {
          minute += 1;
          if (minute > 59) {
            minute = minute - 60;
          }
        }
        //////// Clockmode module
        if (strcmp(module_type, "ClockMode") == 0) {
          clock_mode = clock_mode + 1;
          if (clock_mode == 2) {
            clock_mode = 0;
          }
        }
        //////// Wristmode module
        if (strcmp(module_type, "WristMode") == 0) {
          wrist_mode = wrist_mode + 1;
          if (wrist_mode == 2) {
            wrist_mode = 0;
          }
        }
      }
    }
  }

  if (millis() - press_time > over_time_ms) {
    options_mode = false;
    return false;
  }

  return true;
}

/////////////////////////////////////////////////////////////////// Modules
void Mod_Battery() {
  int threshold_ms = 500;
  static unsigned long press_time = millis();
  static int charge = analogRead(Bat_READ);

  while (options_section == true) {
    Mod_Charge();
    options_section = Module_Timer(1000, "Battery");

    if (millis() - press_time > threshold_ms) {
      charge = analogRead(Bat_READ);
      press_time = millis();
    }

    float voltage = charge * (3.0 / 1023.0);  // assuming 3.3V reference
    int charge_percent;

    if (voltage <= 2.0) {
      charge_percent = 0;
      digitalWrite(Bat_ON, LOW);
    } else if (voltage >= 3.0) {
      charge_percent = 99;
    } else {
      charge_percent = (voltage - 2.0) * (99.0 / (3.0 - 2.0));
    }

    charge_percent = map(charge_percent, 20, 99, 0, 99);
    if (charge_percent < 1) charge_percent = 1;

    int charge_dig_l = charge_percent / 10;
    int charge_dig_r = charge_percent % 10;


    for (uint16_t colCnt = 0; colCnt < large_seg7_matrix[charge_dig_l].numElements; colCnt++) {
      LED_ON(large_seg7_matrix[charge_dig_l].pins[colCnt] - 3, brightness, 100, false);
    }
    for (uint16_t colCnt = 0; colCnt < large_seg7_matrix[charge_dig_r].numElements; colCnt++) {
      LED_ON(large_seg7_matrix[charge_dig_r].pins[colCnt] + 3, brightness, 100, false);
    }
  }
}

void Mod_Brightness() {
  while (options_section == true) {
    Mod_Charge();
    options_section = Module_Timer(1000, "Brightness");
    for (uint16_t colCnt = 0; colCnt < large_seg7_matrix[(brightness - 10) / 10].numElements; colCnt++) {
      LED_ON(large_seg7_matrix[(brightness - 10) / 10].pins[colCnt], brightness, 100, false);
    }
  }
}

void Mod_Hour() {
  unsigned long hour_flash_timer = millis();  // milliseconds
  int flash_switch = true;
  while (options_section == true) {
    Mod_Charge();
    options_section = Module_Timer(1000, "Hour");
    int hour_dig_r = hour % 10;
    int hour_dig_l = (hour - hour_dig_r) / 10;
    int min_dig_r = minute % 10;
    int min_dig_l = (minute - minute % 10) / 10;
    if (millis() - hour_flash_timer > 375 && flash_switch == false) {
      flash_switch = true;
      hour_flash_timer = millis();
    }
    if (millis() - hour_flash_timer > 375 && flash_switch == true) {
      flash_switch = false;
      hour_flash_timer = millis();
    }

    if (flash_switch == true) {
      for (uint16_t colCnt = 0; colCnt < small_seg7_matrix[hour_dig_l].numElements; colCnt++) {
        LED_ON(small_seg7_matrix[hour_dig_l].pins[colCnt] - 6, brightness, 100, false);
      }
      for (uint16_t colCnt = 0; colCnt < small_seg7_matrix[hour_dig_r].numElements; colCnt++) {
        LED_ON(small_seg7_matrix[hour_dig_r].pins[colCnt] - 2, brightness, 100, false);
      }
    } else {
      for (uint16_t colCnt = 0; colCnt < small_seg7_matrix[hour_dig_l].numElements; colCnt++) {
        LED_ON(small_seg7_matrix[hour_dig_l].pins[colCnt] - 6, brightness, 100, true);
      }
      for (uint16_t colCnt = 0; colCnt < small_seg7_matrix[hour_dig_r].numElements; colCnt++) {
        LED_ON(small_seg7_matrix[hour_dig_r].pins[colCnt] - 2, brightness, 100, true);
      }
    }
    for (uint16_t colCnt = 0; colCnt < small_seg7_matrix[min_dig_l].numElements; colCnt++) {
      LED_ON(small_seg7_matrix[min_dig_l].pins[colCnt] + 2, brightness, 100, false);
    }
    for (uint16_t colCnt = 0; colCnt < small_seg7_matrix[min_dig_r].numElements; colCnt++) {
      LED_ON(small_seg7_matrix[min_dig_r].pins[colCnt] + 6, brightness, 100, false);
    }
  }
}

void Mod_Minute() {
  unsigned long minute_flash_timer = millis();  // milliseconds
  int flash_switch = true;
  while (options_section == true) {
    Mod_Charge();
    options_section = Module_Timer(1000, "Minute");
    int hour_dig_r = hour % 10;
    int hour_dig_l = (hour - hour_dig_r) / 10;
    int min_dig_r = minute % 10;
    int min_dig_l = (minute - minute % 10) / 10;
    if (millis() - minute_flash_timer > 375 && flash_switch == false) {
      flash_switch = true;
      minute_flash_timer = millis();
    }
    if (millis() - minute_flash_timer > 375 && flash_switch == true) {
      flash_switch = false;
      minute_flash_timer = millis();
    }

    if (flash_switch == true) {
      for (uint16_t colCnt = 0; colCnt < small_seg7_matrix[min_dig_l].numElements; colCnt++) {
        LED_ON(small_seg7_matrix[min_dig_l].pins[colCnt] + 2, brightness, 100, false);
      }
      for (uint16_t colCnt = 0; colCnt < small_seg7_matrix[min_dig_r].numElements; colCnt++) {
        LED_ON(small_seg7_matrix[min_dig_r].pins[colCnt] + 6, brightness, 100, false);
      }
    } else {
      for (uint16_t colCnt = 0; colCnt < small_seg7_matrix[min_dig_l].numElements; colCnt++) {
        LED_ON(small_seg7_matrix[min_dig_l].pins[colCnt] + 2, brightness, 100, true);
      }
      for (uint16_t colCnt = 0; colCnt < small_seg7_matrix[min_dig_r].numElements; colCnt++) {
        LED_ON(small_seg7_matrix[min_dig_r].pins[colCnt] + 6, brightness, 100, true);
      }
    }
    for (uint16_t colCnt = 0; colCnt < small_seg7_matrix[hour_dig_l].numElements; colCnt++) {
      LED_ON(small_seg7_matrix[hour_dig_l].pins[colCnt] - 6, brightness, 100, false);
    }
    for (uint16_t colCnt = 0; colCnt < small_seg7_matrix[hour_dig_r].numElements; colCnt++) {
      LED_ON(small_seg7_matrix[hour_dig_r].pins[colCnt] - 2, brightness, 100, false);
    }
  }
}

void Mod_WristMode() {
  while (options_section == true) {
    Mod_Charge();
    options_section = Module_Timer(1000, "WristMode");
    for (uint16_t colCnt = 0; colCnt < LEFT_RIGHT_matrix[wrist_mode].numElements; colCnt++) {
      LED_ON(LEFT_RIGHT_matrix[wrist_mode].pins[colCnt], brightness, 100, false);
    }
  }
}

void Mod_ClockMode() {
  while (options_section == true) {
    Mod_Charge();
    options_section = Module_Timer(1000, "ClockMode");
    for (uint16_t colCnt = 0; colCnt < PING4DIG_matrix[clock_mode].numElements; colCnt++) {
      LED_ON(PING4DIG_matrix[clock_mode].pins[colCnt], brightness, 100, false);
    }
  }
}

#endif