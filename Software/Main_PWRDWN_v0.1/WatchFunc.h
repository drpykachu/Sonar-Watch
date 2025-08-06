// WatchFunc.h

#ifndef WatchFunc_H
#define WatchFunc_H

//////////////////////////////////////////////////////////////// LED CONTROL FUNCTION

extern const int Button;
extern int LED_flip;
extern int wrist_mode;
extern const int Bat_READ;
extern const int Bat_ON;
extern int brightness;
extern const int Charge_STAT;

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
  if (wrist_mode == 1){
    LED_input = 200 - LED;
  }
  else{
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
  if(dummy == false){
    digitalWrite(high_pin, HIGH);
  }
  else{
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

void All_flash(float duration, int brightness, int repeat){

  for(int i =0; i < repeat; i++){
    unsigned long flash_start = millis();
    while (millis() - flash_start < duration*1000) {
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



/////////////////////////////////////////////////////////////////// Battery Logic
void Mod_Battery(int threshold_ms, int display) {
  static unsigned long press_time = millis();
  static int charge = analogRead(Bat_READ);

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

  if (display) {
    for (uint16_t colCnt = 0; colCnt < large_seg7_matrix[charge_dig_l].numElements; colCnt++) {
      LED_ON(large_seg7_matrix[charge_dig_l].pins[colCnt] - 3, brightness, 100, false);
    }
    for (uint16_t colCnt = 0; colCnt < large_seg7_matrix[charge_dig_r].numElements; colCnt++) {
      LED_ON(large_seg7_matrix[charge_dig_r].pins[colCnt] + 3, brightness, 100, false);
    }
  }
}

void Mod_Charge() {
  if (digitalRead(Charge_STAT) == LOW) {
    LED_ON(1, 5, 1000, false);
  }
}



#endif