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
const int Button = 22;  // PF2
const int RTC_WAKE = 23;
const int Charge_STAT = 14;
const int Bat_READ = A11;  // 25
const int SDA_PIN = 32;

// === RTC ===
RTC_DS3231 rtc;
DateTime now;

// === Settings ===
int hour = 1;
int minute = 1;
int second = 1;
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

int NaN = -1;
int life_start = true;
int press_count = 0;

void setup() {
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  pinMode(Bat_READ, INPUT);

  pinMode(RTC_WAKE, OUTPUT);
  digitalWrite(RTC_WAKE, HIGH);

  pinMode(Bat_ON, OUTPUT);
  digitalWrite(Bat_ON, HIGH);  // Keeps regulator ON initially

  // --- Button interrupt setup (rising edge) ---
  pinMode(Button, INPUT_PULLUP);
  PORTF.PIN2CTRL = PORT_ISC_RISING_gc;
  PORTF.INTFLAGS = PIN2_bm;

  // --- Charger status interrupt setup (falling edge) ---
  pinMode(Charge_STAT, INPUT_PULLUP);
  PORTD.PIN2CTRL = PORT_ISC_FALLING_gc;
  PORTD.INTFLAGS = PIN2_bm;

  // === Configure Voltage Level Monitor (VLM) ===
  // Set threshold level to 2.6V (BODLEVEL1)
  // Use “below” mode — triggers when Vcc falls below this level
  // BOD.VLMCTRLA = BOD_LVL_BODLEVEL2_gc | BOD_VLMCFG_BELOW_gc;
  BOD.VLMCTRLA = BOD_LVL_BODLEVEL2_gc | BOD_VLMCFG_ABOVE_gc | BOD_VLMLVL_15ABOVE_gc;

  // Enable VLM interrupt 
  BOD.INTCTRL = BOD_VLMIE_bm;

  // Clear any existing flags
  BOD.INTFLAGS = BOD_VLMIF_bm;

  // --- Sleep setup ---
  SLPCTRL.CTRLA = SLPCTRL_SMODE_PDOWN_gc | SLPCTRL_SEN_bm;
  sei();  // Enable global interrupts
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

    digitalWrite(RTC_WAKE, LOW);  // powers up DS3231
    delay(50);
    now = rtc.now();
    hour = now.hour();
    minute = now.minute();
    digitalWrite(RTC_WAKE, HIGH);  // powers down DS3231

    if (hour >= 12) hour -= 12;

    if (clock_mode == 0)
      Ring_Execute(hour, minute);
    else
      Clock_Execute(hour, minute);
  }

  options_mode = Hold_Timer(1000);
  if (options_mode == true) {
    All_flash(0.05, 100, 3);

    if (options_mode = true) { Mod_Brightness(); options_section = true; }
    if (options_mode = true) { Mod_Hour(); options_section = true; }
    if (options_mode = true) { Mod_Minute(); options_section = true; }
    if (options_mode = true) { Mod_ClockMode(); options_section = true; }
    if (options_mode = true) { Mod_WristMode(); options_section = true; }

    options_mode = false;
    All_flash(0.05, 100, 3);
    Crab_Timer(2000);
    delay(50);
  }

  __asm__ __volatile__("sleep");
}

// === Button Interrupt (PF2) ===
ISR(PORTF_PORT_vect) {
  if (!(VPORTF.IN & (1 << 2))) return;
  PORTF.INTFLAGS = PIN2_bm;
  wake_flag = true;
}

// === Charger Status Interrupt ===
ISR(PORTD_PORT_vect) {
  if (VPORTD.IN & (1 << 2)) return;
  PORTD.INTFLAGS = PIN2_bm;
  CHG_flag = true;
}

// === Voltage Level Monitor Interrupt ===
ISR(BOD_VLM_vect) {
  // Clear interrupt flag
  BOD.INTFLAGS = BOD_VLMIF_bm;

  // === Low voltage detected! ===
  // Cut regulator power immediately
  digitalWrite(Bat_ON, LOW);

  // Optional: enter infinite sleep to reduce current
  // SLPCTRL.CTRLA = SLPCTRL_SMODE_PDOWN_gc | SLPCTRL_SEN_bm;
  __asm__ __volatile__("sleep");
}
