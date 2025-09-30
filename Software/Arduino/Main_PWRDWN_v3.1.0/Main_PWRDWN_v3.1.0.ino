#include <avr/sleep.h>
#include <avr/interrupt.h>
#include "RTClib.h"
#include "WatchData.h"
#include "WatchFunc.h"
#include <string.h>

// === Flags ===
volatile bool wake_flag = false;
volatile bool CHG_flag = false;

// === Pins ===

const int SCL_PIN = 3;


const int Bat_ON = 21;
const int Button = 22;  // PF2 (pin 2 on Arduino pinout — confirm on your board)
const int RTC_WAKE = 23;
const int Charge_STAT = 14;
const int Bat_READ = A11; // 25

const int SDA_PIN = 32;


// === RTC ===
RTC_DS3231 rtc;
DateTime now;

// === Settings ===
int hour = 1;
int minute = 1;
bool options_mode = false;
bool options_section = true;
int clock_mode = 0;
int wrist_mode = 0;
int LED_flip = 0;
int brightness = 50;
int charge = 1;
unsigned long fade_duration = 1000;
unsigned long charge_timer = 1000;

unsigned long lastBatteryUpdate = 0;
const unsigned long batteryInterval = 60 * 60 * 1000;

void setup() {
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  pinMode(Bat_READ, INPUT);
  // pinMode(Charge_STAT, INPUT);

  pinMode(RTC_WAKE, OUTPUT);
  pinMode(RTC_WAKE, HIGH);

  pinMode(Bat_ON, OUTPUT);
  digitalWrite(Bat_ON, HIGH);

  // --- Button interrupt setup (rising edge) ---
  pinMode(Button, INPUT_PULLUP);        // Button connected to PF2 with pullup
  PORTF.PIN2CTRL = PORT_ISC_RISING_gc;  // Enable interrupt on rising edge
  PORTF.INTFLAGS = PIN2_bm;             // Clear any pending interrupt

  // --- Button interrupt setup (rising edge) ---
  pinMode(Charge_STAT, INPUT_PULLUP);        // Button connected to PF2 with pullup
  PORTD.PIN2CTRL = PORT_ISC_FALLING_gc;  // Enable interrupt on rising edge
  PORTD.INTFLAGS = PIN2_bm;             // Clear any pending interrupt

  // --- Sleep setup ---
  SLPCTRL.CTRLA = SLPCTRL_SMODE_PDOWN_gc | SLPCTRL_SEN_bm;  // Power-down sleep
  sei();                                                    // Enable global interrupts
}

void loop() {

  // --- Wake from charger ---
  if (CHG_flag) {
    CHG_flag = false;
    CHG_flash(2);
    wake_flag = false;
  }
  // --- Wake from button ---
  if (wake_flag) {
    wake_flag = false;

    pinMode(RTC_WAKE, LOW);  // powers up DS3231
    delay(50);               // slight delay to give the DS3231 some time to wake up
    now = rtc.now();
    hour = now.hour();
    minute = now.minute();
    pinMode(RTC_WAKE, HIGH);  // powers down DS3231

    if (hour >= 12) {
      hour = hour - 12;
    }

    if (clock_mode == 0) {
      Ring_Execute(hour, minute);
    } else {
      Clock_Execute(hour, minute);
    }
    
  }
  options_mode = Hold_Timer(1000);
  if (options_mode == true) {

    All_flash(0.05, 100, 3);

    //////////// Battery module
    if (options_mode = true){
     Mod_Battery();    options_section = true;}
    //////////// Brightness module
    if (options_mode = true){
      Mod_Brightness(); options_section = true;}
    //////////// Hour module
    if (options_mode = true){
      Mod_Hour();    options_section = true;}
    //////////// Minute Module
    if (options_mode = true){
      Mod_Minute();    options_section = true;}
    ////////// Clocktype Module
    if (options_mode = true){
      Mod_ClockMode();    options_section = true;}

    ////////// Wristtype Module
    if (options_mode = true){
      Mod_WristMode();    options_section = true;}

    ////////////// Closes Modules
    options_mode = false;
    All_flash(0.05, 100, 3);

    Crab_Timer(2000);

    delay(50);
  }

  // Enter power-down sleep - wakes only on async interrupt (PF2 rising)
  __asm__ __volatile__("sleep");
}

// === Button Interrupt (PF2) ===
ISR(PORTF_PORT_vect) {
  if (!(VPORTF.IN & (1 << 2))) return;  // Only act on rising edge
  PORTF.INTFLAGS = PIN2_bm;             // Clear interrupt flag
  wake_flag = true;
}

ISR(PORTD_PORT_vect) {
  if (VPORTD.IN & (1 << 2)) return;   // Only act on falling edge
  PORTD.INTFLAGS = PIN2_bm;           // Clear interrupt flag
  CHG_flag = true;
}

