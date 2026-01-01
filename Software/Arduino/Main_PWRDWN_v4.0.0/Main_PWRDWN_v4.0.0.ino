#include <avr/sleep.h>
#include <avr/interrupt.h>
#include "RTClib.h"
#include "WatchData.h"
#include "WatchFunc.h"
#include <string.h>

// === Flags ===
volatile bool wake_flag = false;
volatile bool CHG_flag = false;
volatile bool Minute_flag = false;

// === Battery timing ===
volatile uint8_t sec_count = 0;

// === Pins ===
const int SCL_PIN = 3;
const int Bat_READ_GND = 20;
const int Bat_ON = 21;
const int Button = 22;    // PF2
const int RTC_WAKE = 23;
const int Charge_STAT = 14;
const int Bat_READ = A11; // Pin 25
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
int read_timer = 60; 
unsigned long fade_duration = 1000;
unsigned long charge_timer = 1000;

unsigned long time_unplugged = 0;

unsigned long lastBatteryUpdate = 0;
const unsigned long batteryInterval = 60 * 60 * 1000;

int NaN = -1;
int life_start = true;
int press_count = 0;
float volt_cutoff = 2.2 ;

// === Setup ===
void setup() {
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  pinMode(Bat_READ, INPUT);
  pinMode(Bat_READ_GND, INPUT);

  pinMode(RTC_WAKE, OUTPUT);
  pinMode(RTC_WAKE, HIGH);

  pinMode(Bat_ON, OUTPUT);
  digitalWrite(Bat_ON, HIGH);

  // --- Button interrupt setup (rising edge) ---
  pinMode(Button, INPUT_PULLUP);
  PORTF.PIN2CTRL = PORT_ISC_RISING_gc;
  PORTF.INTFLAGS = PIN2_bm;

  // --- Charger status interrupt (falling edge) ---
  pinMode(Charge_STAT, INPUT_PULLUP);
  PORTD.PIN2CTRL = PORT_ISC_FALLING_gc;
  PORTD.INTFLAGS = PIN2_bm;

  // --- Internal RTC PIT wake every minute (via 1-second PIT counting) ---
  while (RTC.STATUS > 0) {}  // Wait for sync

  // Enable RTC and run in standby; 1-second PIT tick (using 32.768 kHz source)
  RTC.CTRLA = RTC_RUNSTDBY_bm | RTC_PRESCALER_DIV1_gc | RTC_RTCEN_bm;
  RTC.PITCTRLA = RTC_PERIOD_CYC32768_gc | RTC_PITEN_bm;  // 1-second tick
  RTC.PITINTCTRL = RTC_PI_bm;                             // Enable PIT interrupt

  // --- Sleep setup: use power-down mode ---
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  SLPCTRL.CTRLA = SLPCTRL_SMODE_PDOWN_gc | SLPCTRL_SEN_bm;

  sei();  // Enable global interrupts
}


// === Main Loop ===
void loop() {

  // --- 1-minute battery reading ---
  if (Minute_flag) {
    Minute_flag = false;


    int charge_2 = analogRead(Bat_READ);
    float voltage_2 = charge_2 * (3.0 / 1023.0);  // assuming 3.3V reference
    float charge_percent_2 = batteryPercent(voltage_2);
    read_timer = charge_percent_2;
    
    for(int i =0; i < 500; i++){
      LED_ON(1, 5, 1000, false); // for testing purposes
    }

    // Power the divider briefly
    pinMode(Bat_READ_GND, OUTPUT);
    digitalWrite(Bat_READ_GND, LOW);
    // Remove load from divider
    pinMode(Bat_READ_GND, INPUT);
    delay(50);                         // allow voltage to settle


    if (analogRead(Bat_READ) * (3.0 / 1023.0) <= volt_cutoff) {
      BYE_flash(0.5, 100, 2);
      digitalWrite(Bat_ON, HIGH); //shuts it off
    }

    delay(50);                         // allow voltage to settle

    digitalWrite(Bat_ON, HIGH); //brings it back when its plugged in, if 

  }

  // --- Wake from charger ---
  if (CHG_flag) {
    CHG_flash(2);
    CHG_flag = false;
    wake_flag = false;
  }

  // --- Wake from button ---
  if (wake_flag) {
    pinMode(RTC_WAKE, LOW);
    delay(50);
    now = rtc.now();
    hour = now.hour();
    minute = now.minute();
    pinMode(RTC_WAKE, HIGH);

    if (hour >= 12) hour -= 12;

    if (clock_mode == 0) {
      Ring_Execute(hour, minute);
    } else {
      Clock_Execute(hour, minute);
    }
    wake_flag = false;
  }

  // --- Options mode ---
  options_mode = Hold_Timer(1000);
  if (options_mode == true) {

    All_flash(0.05, 100, 3);

    // Note: your code uses assignment in the if()s previously; I preserved original logic
    if (options_mode = true) { press_count = -1; Mod_Battery();   options_section = true; }
    if (options_mode = true) { Mod_Brightness(); options_section = true; }
    if (options_mode = true) { Mod_Hour();       options_section = true; }
    if (options_mode = true) { Mod_Minute();     options_section = true; }
    if (options_mode = true) { Mod_ClockMode();  options_section = true; }
    if (options_mode = true) { Mod_WristMode();  options_section = true; }

    options_mode = false;
    All_flash(0.05, 100, 3);
    Crab_Timer(2000);
    delay(50);
    wake_flag = false;
  }

  // === GO TO SLEEP (with BOD disabled for the sleep window) ===
  // Use avr-libc provided macro to correctly disable BOD timing sequence.
  sleep_enable();            // set SE bit
#if defined(sleep_bod_disable)
  sleep_bod_disable();       // safely disable BOD for the upcoming sleep period
#endif
  // Important ordering: enable interrupts then enter sleep so we won't miss an ISR
  sei();
  sleep_cpu();               // execute SLEEP instruction
  sleep_disable();           // clear SE bit after wake
}



// === Button Interrupt (PF2) ===
ISR(PORTF_PORT_vect) {
  if (!(VPORTF.IN & (1 << 2))) return;
  PORTF.INTFLAGS = PIN2_bm;
  wake_flag = true;
}

// === Charger status interrupt (PD2) ===
ISR(PORTD_PORT_vect) {
  if (VPORTD.IN & (1 << 2)) return;
  PORTD.INTFLAGS = PIN2_bm;
  CHG_flag = true;
}

// === Internal RTC PIT interrupt (1-second tick) ===
ISR(RTC_PIT_vect) {
  RTC.PITINTFLAGS = RTC_PI_bm;
  
  sec_count++;
  if (sec_count >= read_timer) {
    sec_count = 0;
    Minute_flag = true;
  }
}
