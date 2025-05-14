const int pins[6] = {PA8, PF1, PF0, PB6, PA15, PB7};  // D9 to D4


const char* Charlie[5][5][6] = {
  {{"LOW", "HIGH", "INPUT", "INPUT", "INPUT", "INPUT"},     // 1.1
   {"LOW", "INPUT", "HIGH", "INPUT", "INPUT", "INPUT"},     // 1.2
   {"HIGH", "INPUT", "LOW", "INPUT", "INPUT", "INPUT"},     // 1.3
   {"INPUT", "INPUT", "LOW", "INPUT", "HIGH", "INPUT"},     // 1.4
   {"INPUT", "INPUT", "HIGH", "INPUT", "LOW", "INPUT"}},    // 1.5

  {{"INPUT", "LOW", "HIGH", "INPUT", "INPUT", "INPUT"},     // 2.1
   {"INPUT", "HIGH", "LOW", "INPUT", "INPUT", "INPUT"},     // 2.2
   {"LOW", "INPUT", "INPUT", "INPUT", "HIGH", "INPUT"},     // 2.3
   {"HIGH", "INPUT", "INPUT", "INPUT", "LOW", "INPUT"},     // 2.4
   {"INPUT", "INPUT", "LOW", "INPUT", "INPUT", "HIGH"}},    // 2.5

  {{"INPUT", "INPUT", "LOW", "HIGH", "INPUT", "INPUT"},     // 3.1
   {"INPUT", "LOW", "INPUT", "INPUT", "HIGH", "INPUT"},     // 3.2
   {"INPUT", "HIGH", "INPUT", "INPUT", "LOW", "INPUT"},     // 3.3
   {"LOW", "INPUT", "INPUT", "INPUT", "INPUT", "HIGH"},     // 3.4
   {"HIGH", "INPUT", "INPUT", "INPUT", "INPUT", "LOW"}},    // 3.5

  {{"INPUT", "INPUT", "INPUT", "LOW", "HIGH", "INPUT"},     // 4.1
   {"INPUT", "INPUT", "INPUT", "HIGH", "LOW", "INPUT"},     // 4.2
   {"INPUT", "LOW", "INPUT", "INPUT", "INPUT", "HIGH"},     // 4.3
   {"INPUT", "HIGH", "INPUT", "INPUT", "INPUT", "LOW"},     // 4.4
   {"LOW", "INPUT", "INPUT", "HIGH", "INPUT", "INPUT"}},    // 4.5

  {{"INPUT", "INPUT", "INPUT", "INPUT", "LOW", "HIGH"},     // 5.1
   {"INPUT", "INPUT", "INPUT", "LOW", "INPUT", "HIGH"},     // 5.2
   {"INPUT", "INPUT", "INPUT", "HIGH", "INPUT", "LOW"},     // 5.3
   {"INPUT", "LOW", "INPUT", "HIGH", "INPUT", "INPUT"},     // 5.4
   {"INPUT", "HIGH", "INPUT", "LOW", "INPUT", "INPUT"}}     // 5.5
};

int row = 0; // LOW
int col = 4; // HIGH

void setup() {
  Serial.begin(9600);
  PinReset();
}

void loop() {
  const char** netRow = Charlie[row][col];

  for (int i = 0; i < 6; i++) {
    int pin = pins[i];
    const char* mode = netRow[i];

    if (strcmp(mode, "LOW") == 0) {
      pinMode(pin, OUTPUT);
      digitalWrite(pin, LOW);
    } else if (strcmp(mode, "HIGH") == 0) {
      pinMode(pin, OUTPUT);
      digitalWrite(pin, HIGH);
    } else {
      pinMode(pin, INPUT);
    }
  }

  delayMicroseconds(150);
  PinReset();
  // delay(10);
}

void PinReset() {
  for (int i = 0; i < 6; i++) {
    pinMode(pins[i], INPUT);
  }
}
