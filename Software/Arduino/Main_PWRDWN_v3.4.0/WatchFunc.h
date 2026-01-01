// WatchFunc.h

#ifndef WatchFunc_H
#define WatchFunc_H

extern const int Button;
extern int LED_flip;
extern int wrist_mode;
extern const int Bat_READ;
extern const int Bat_READ_GND;
extern const int Bat_ON;
extern int brightness;
extern const int Charge_STAT;
extern DateTime now;
extern RTC_DS3231 rtc;
extern int hour;
extern int minute;
extern int second;
extern int clock_mode;
extern unsigned long fade_duration;
extern bool options_section;
extern bool options_mode;
extern int life_start;
extern const int RTC_WAKE;
extern int NaN;
extern int alive_xy[15][15];
extern int life_xy[15][15];
extern int press_count;
extern float volt_cutoff;

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

/////////////////////////////////// LIFE
void Life() {
  while( digitalRead(Button) == LOW) {
    if (life_start == true) {
      pinMode(RTC_WAKE, LOW);  // powers up DS3231
      delay(50);
      second = now.second();
      pinMode(RTC_WAKE, HIGH);  // powers down DS3231

      // random seed (use analog pin + time for entropy)
      randomSeed(analogRead(A0) * micros());  // random fill
      float fillRatio = 0.6;                  // 0.5 = 50%, 0.3 = 30%, etc.
      for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 15; j++) {
          if (!isnan(alive_xy[i][j])) {
            alive_xy[i][j] = (random(100) < fillRatio * 100) ? 1 : 0;
          }
        }
      }
      life_start = false;
    }


    int alive_xy_next[15][15] = {
      { NaN, NaN, NaN, 0, 0, 0, 0, 0, 0, 0, 0, 0, NaN, NaN, NaN },  // 0
      { NaN, NaN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NaN, NaN },      // 1
      { NaN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NaN },          // 2
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },              // 3
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },              // 4
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },              // 5
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },              // 6
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },              // 7
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },              // 8
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },              // 9
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },              // 10
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },              // 11
      { NaN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NaN },          // 12
      { NaN, NaN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NaN, NaN },      // 13
      { NaN, NaN, NaN, 0, 0, 0, 0, 0, 0, 0, 0, 0, NaN, NaN, NaN }   // 14
    };

    int neighbors_alive[15][15] = {
      { NaN, NaN, NaN, 0, 0, 0, 0, 0, 0, 0, 0, 0, NaN, NaN, NaN },  // 0
      { NaN, NaN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NaN, NaN },      // 1
      { NaN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NaN },          // 2
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },              // 3
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },              // 4
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },              // 5
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },              // 6
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },              // 7
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },              // 8
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },              // 9
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },              // 10
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },              // 11
      { NaN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NaN },          // 12
      { NaN, NaN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NaN, NaN },      // 13
      { NaN, NaN, NaN, 0, 0, 0, 0, 0, 0, 0, 0, 0, NaN, NaN, NaN }   // 14
    };


    // counts number alive
    int num_alive = 0;
    for (int row = 0; row < 15; row++) {
      for (int col = 0; col < 15; col++) {
        if (alive_xy[row][col] == 1) {
          num_alive = num_alive + 1;
        }
      }
    }

    // Displays grid of life
    for (int waiter = 0; waiter < 1000 / num_alive; waiter++) {
      for (int row = 0; row < 15; row++) {
        for (int col = 0; col < 15; col++) {
          if (alive_xy[row][col] == 1) {
            LED_ON(life_xy[row][col] + 1, brightness, 100, false);
          }
        }
      }
    }

    // finding neighbors
    for (int row = 0; row < 15; row++) {
      for (int col = 0; col < 15; col++) {
        // identifies grid around each pixel
        if (alive_xy[row][col] != NaN) {
          int b0_0;
          int b0_1;
          int b0_2;
          int b1_0;
          int b1_2;
          int b2_0;
          int b2_1;
          int b2_2;

          b0_0 = alive_xy[row - 1][col - 1];
          b0_1 = alive_xy[row - 1][col];
          b0_2 = alive_xy[row - 1][col + 1];

          b1_0 = alive_xy[row][col - 1];
          b1_2 = alive_xy[row][col + 1];

          b2_0 = alive_xy[row + 1][col - 1];
          b2_1 = alive_xy[row + 1][col];
          b2_2 = alive_xy[row + 1][col + 1];

          if ((row - 1) < 0) {
            b0_0 = 0;
            b0_1 = 0;
            b0_2 = 0;
          }

          if ((row + 1) >= 15) {
            b2_0 = 0;
            b2_1 = 0;
            b2_2 = 0;
          }

          if ((col - 1) < 0) {
            b0_0 = 0;
            b1_0 = 0;
            b2_0 = 0;
          }

          if ((col + 1) >= 15) {
            b0_2 = 0;
            b1_2 = 0;
            b2_2 = 0;
          }

          ////////////// removes negative
          if (b0_0 < 0) {
            b0_0 = 0;
          }
          if (b0_1 < 0) {
            b0_1 = 0;
          }
          if (b0_2 < 0) {
            b0_2 = 0;
          }
          if (b1_0 < 0) {
            b1_0 = 0;
          }
          if (b1_2 < 0) {
            b1_2 = 0;
          }
          if (b2_0 < 0) {
            b2_0 = 0;
          }
          if (b2_1 < 0) {
            b2_1 = 0;
          }
          if (b2_2 < 0) {
            b2_2 = 0;
          }
          neighbors_alive[row][col] = b0_0 + b0_1 + b0_2 + b1_0 + b1_2 + b2_0 + b2_1 + b2_2;
        }
      }
    }
    ///////////////////////// HERE LIES THE RULES OF LIFE /////////////////////////

    for (int row = 0; row < 15; row++) {
      for (int col = 0; col < 15; col++) {
        if (alive_xy[row][col] != NaN) {
          int life_death = neighbors_alive[row][col];
          // Any live cell with fewer than two live neighbours dies, as if by underpopulation.
          if (alive_xy[row][col] == 1 && life_death < 2) {
            alive_xy_next[row][col] = 0;
          }
          // Any live cell with two or three live neighbours lives on to the next generation.
          if (alive_xy[row][col] == 1 && life_death == 2 || alive_xy[row][col] == 1 && life_death == 3) {
            alive_xy_next[row][col] = 1;
          }

          // Any live cell with more than three live neighbours dies, as if by overpopulation.
          if (alive_xy[row][col] == 1 && life_death > 3) {
            alive_xy_next[row][col] = 0;
          }
          // Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
          if (alive_xy[row][col] == 0 && life_death == 3) {
            alive_xy_next[row][col] = 1;
          }
        }
      }
    }

    // sets the next generation
    for (int i = 0; i < 15; i++) {
      for (int j = 0; j < 15; j++) {
        alive_xy[i][j] = alive_xy_next[i][j];
      }
    }
  }
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


//////////////////////////////////////////////////////////////// Flash BYE

void BYE_flash(float duration, int brightness, int repeat) {
 
  const int BYE[] = {49, 50, 51, 54, 58, 60, 61, 62, 63, 64, 67, 69, 73, 75, 79, 82, 85, 87,
  90, 94, 96, 101, 105, 106, 107, 108, 109, 112, 116, 120, 124, 127, 131, 135, 139, 140, 141, 146, 150, 151, 152, 153};


  RINGS BYEMatrix[] =
  {  {BYE, NUMELEMENTS(BYE)}};


  for (int i = 0; i < repeat; i++) {
    unsigned long flash_start = millis();
    while (millis() - flash_start < duration * 1000) {
      for (int f = 0; f < BYEMatrix[0].numElements; f++) {
        LED_ON(BYEMatrix[0].pins[f], brightness, 50, false);
      }
    }
    delay(100);
  }
}


//////////////////////////////////////////////////////////////// Flash ALL
void All_flash(float duration, int brightness, int repeat) {
 
  const int sans[] = {2, 3, 4, 5, 6, 7, 8, 10, 11, 19, 20, 21, 33, 35, 47, 49, 50, 63, 64, 67, 68, 69, 
  73, 74, 75, 78, 79, 82, 83, 84, 88, 89, 90, 93, 94, 95, 97, 98, 99, 101, 103, 104, 105, 107, 108, 111,
  115, 116, 117, 121, 124, 125, 127, 135, 137, 138, 139, 142, 143, 144, 145, 146, 147, 148, 149, 150, 153, 
  154, 155, 158, 160, 162, 164, 167, 168, 169, 170, 173, 174, 175, 176, 177, 180, 181, 182, 183, 184, 190, 
  191, 192, 194, 195, 196, 197, 198, 199, 200};


  RINGS SansMatrix[] =
  {  {sans, NUMELEMENTS(sans)}};


  for (int i = 0; i < repeat; i++) {
    unsigned long flash_start = millis();

    while (millis() - flash_start < duration * 1000) {
      if(i != 1){
        for (int f = 0; f < LED_count; f++) {
          LED_ON(f, brightness, 50, false);
        }
      }
      else{
        for (int f = 0; f < SansMatrix[0].numElements; f++) {
          LED_ON(SansMatrix[0].pins[f], brightness, 50, false);
        }
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
        if (strcmp(module_type, "Battery") == 0) {
          press_count = press_count + 1;
          if (press_count == 5){
            press_count = 0;
            life_start = true;
            Life();
            while(digitalRead(Button) == HIGH){
            }
            press_time = millis();
          }
        }

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

// function to linear interpolate the battery percentage
struct Point {
  float v;   // voltage
  float pct; // percentage
};

const Point curve[] = {
  {3.000, 99},
  {2.960, 98},
  {2.928, 97},
  {2.896, 95},
  {2.873, 93},
  {2.835, 89},
  {2.806, 86},
  {2.759, 82},
  {2.733, 78},
  {2.710, 73},
  {2.677, 68},
  {2.663, 63},
  {2.648, 57},
  {2.640, 51},
  {2.620, 43},
  {2.620, 35},
  {2.617, 28},
  {2.608, 20},
  {2.579, 15},
  {2.547, 11},
  {2.512, 6},
  {2.422, 3},
  {2.286, 0}
};

const int CURVE_SIZE = sizeof(curve) / sizeof(curve[0]);
int batteryPercent(float voltage) {
  // Clamp high
  if (voltage >= curve[0].v) return (int)curve[0].pct;

  // Clamp low
  if (voltage <= curve[CURVE_SIZE - 1].v) return (int)curve[CURVE_SIZE - 1].pct;

  // Find interval
  for (int i = 0; i < CURVE_SIZE - 1; i++) {
    if (voltage <= curve[i].v && voltage >= curve[i + 1].v) {

      float v1 = curve[i].v;
      float p1 = curve[i].pct;
      float v2 = curve[i + 1].v;
      float p2 = curve[i + 1].pct;

      // Linear interpolation
      float t = (voltage - v2) / (v1 - v2);
      float pct = p2 + t * (p1 - p2);

      return (int)(pct + 0.5f);  // round to nearest whole %
    }
  }

  return 0;
}

void Mod_Battery() {
  int threshold_ms = 100;
  static unsigned long press_time = millis();

  // sets PFO (Pin 20) from Input to GND (LOW)
  pinMode(Bat_READ_GND, OUTPUT);
  digitalWrite(Bat_READ_GND, LOW);

  // initial read
  delay(50);
  static int charge = analogRead(Bat_READ);


  press_time = millis();

  // Life();
  while (options_section == true) {
    Mod_Charge();
    options_section = Module_Timer(1000, "Battery");

    if (millis() - press_time > threshold_ms) {

      // reads the pin voltage (But in terms of bit value, not voltage)
      charge = analogRead(Bat_READ);
    
      press_time = millis();
    }

    float voltage = charge * (3.0 / 1023.0);  // assuming 3.3V reference
    int charge_percent;

    if (voltage <= volt_cutoff) {
      charge_percent = 0;
      // digitalWrite(Bat_ON, LOW); // VLM should take vare of it
    } else if (voltage >= 3.0) {
      charge_percent = 99;
    } else {
      charge_percent = batteryPercent(voltage);
    }

    if (charge_percent < 1) charge_percent = 0;

    int charge_dig_l = charge_percent / 10;
    int charge_dig_r = charge_percent % 10;


    for (uint16_t colCnt = 0; colCnt < large_seg7_matrix[charge_dig_l].numElements; colCnt++) {
      LED_ON(large_seg7_matrix[charge_dig_l].pins[colCnt] - 3, brightness, 100, false);
    }
    for (uint16_t colCnt = 0; colCnt < large_seg7_matrix[charge_dig_r].numElements; colCnt++) {
      LED_ON(large_seg7_matrix[charge_dig_r].pins[colCnt] + 3, brightness, 100, false);
    }
  }

    // sets PFO (Pin 20) from Input to high impedance to prevent current leakage
  delay(50);
  pinMode(Bat_READ_GND, INPUT);

}

#endif