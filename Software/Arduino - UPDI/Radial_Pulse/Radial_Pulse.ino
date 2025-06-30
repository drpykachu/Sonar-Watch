#define NUMELEMENTS(x) (sizeof(x) / sizeof(x[0]))

const int pin_count = 16;
/////////////////////////////0,1,2,3,4,5,6,7,8 ,9 ,10,11,12,13,14,15
/////////////////////////////A,B,C,D,E,F,G,H,I ,J ,K ,L ,M ,N ,O, P 
const int pins[pin_count] = {4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};  

const int LED_count = 201;

int LOW_POS[] = {
  0, 4, 4, 6, 6, 8, 8, 10, 10, 1,
  4, 0, 6, 4, 8, 6, 10, 8, 12, 10,
  2, 4, 1, 6, 0, 8, 4, 10, 6, 12,
  8, 13, 10, 3, 4, 2, 6, 1, 8, 0,
  10, 4, 12, 6, 13, 8, 14, 10, 4, 3,
  6, 2, 8, 1, 10, 0, 12, 4, 13, 6,
  14, 8, 15, 5, 6, 3, 8, 2, 10, 1,
  12, 0, 13, 4, 14, 6, 15, 8, 6, 5,
  8, 3, 10, 2, 12, 1, 13, 0, 14, 4,
  15, 6, 11, 7, 8, 5, 10, 3, 12, 2,
  13, 1, 14, 0, 15, 4, 11, 6, 8, 7,
  10, 5, 12, 3, 13, 2, 14, 1, 15, 0,
  11, 4, 9, 9, 10, 7, 12, 5, 13, 3,
  14, 2, 15, 1, 11, 0, 9, 4, 10, 9,
  12, 7, 13, 5, 14, 3, 15, 2, 11, 1,
  9, 0, 7, 11, 12, 9, 13, 7, 14, 5,
  15, 3, 11, 2, 9, 1, 7, 0, 11, 13,
  9, 14, 7, 15, 5, 11, 3, 9, 2, 7,
  1, 11, 14, 9, 15, 7, 11, 5, 9, 3,
  7, 2, 11, 15, 9, 11, 7, 9, 5, 7, 3 
  };

int HIGH_POS[] = {
  4, 0, 6, 4, 8, 6, 10, 8, 12, 4,
  1, 6, 0, 8, 4, 10, 6, 12, 8, 13,
  4, 2, 6, 1, 8, 0, 10, 4, 12, 6,
  13, 8, 14, 4, 3, 6, 2, 8, 1, 10,
  0, 12, 4, 13, 6, 14, 8, 15, 5, 6,
  3, 8, 2, 10, 1, 12, 0, 13, 4, 14,
  6, 15, 8, 6, 5, 8, 3, 10, 2, 12,
  1, 13, 0, 14, 4, 15, 6, 11, 7, 8,
  5, 10, 3, 12, 2, 13, 1, 14, 0, 15,
  4, 11, 6, 8, 7, 10, 5, 12, 3, 13,
  2, 14, 1, 15, 0, 11, 4, 9, 9, 10,
  7, 12, 5, 13, 3, 14, 2, 15, 1, 11,
  0, 9, 4, 10, 9, 12, 7, 13, 5, 14,
  3, 15, 2, 11, 1, 9, 0, 7, 11, 12,
  9, 13, 7, 14, 5, 15, 3, 11, 2, 9,
  1, 7, 0, 12, 11, 13, 9, 14, 7, 15,
  5, 11, 3, 9, 2, 7, 1, 5, 13, 11,
  14, 9, 15, 7, 11, 5, 9, 3, 7, 2,
  5, 14, 11, 15, 9, 11, 7, 9, 5, 7,
  3, 5, 15, 11, 11, 9, 9, 7, 7, 5,5
};

int ring0[] = {101};
int ring1[] = {86, 100, 102, 116};
int ring2[] = {71, 85, 87, 99, 103, 115, 117, 131};
int ring3[] = {56, 70, 72, 84, 88, 98, 104, 114, 118, 130, 132, 146};
int ring4[] = {41, 55, 57, 69, 73, 83, 89, 97, 105, 113, 119, 129, 133, 145, 147, 161};
int ring5[] = {27, 40, 42, 54, 58, 68, 74, 82, 90, 96, 106, 112, 120, 128, 134, 144, 148, 160, 162, 175};
int ring6[] = {15, 26, 28, 39, 43, 53, 59, 67, 75, 81, 91, 95, 107, 111, 121, 127, 135, 143, 149, 159, 163, 174, 176, 187};
int ring7[] = {5, 14, 16, 25, 29, 38, 44, 52, 60, 66, 76, 80, 92, 94, 108, 110, 122, 126, 136, 142, 150, 158, 164, 173, 177, 186, 188, 197};
int ring8[] = {4, 6, 13, 17, 24, 30, 37, 45, 51, 61, 65, 77, 79, 93, 109, 123, 125, 137, 141, 151, 157, 165, 172, 178, 185, 189, 196, 198};
int ring9[] = {3, 7, 12, 18, 23, 31, 36, 46, 50, 62, 64, 78, 124, 138, 140, 152, 156, 166, 171, 179, 184, 190, 195, 199};
int ring10[] = {2, 8, 11, 19, 22, 32, 35, 47, 49, 63, 139, 153, 155, 167, 170, 180, 183, 191, 194, 200};
int ring11[] = {1, 9, 10, 20, 21, 33, 34, 48, 154, 168, 169, 181, 182, 192, 193, 201};


struct RINGS
{
  int *pins;
  uint16_t numElements;
};



RINGS ringMatrix[] =
{
  {ring0, NUMELEMENTS(ring0)},
  {ring1, NUMELEMENTS(ring1)},
  {ring2, NUMELEMENTS(ring2)},
  {ring3, NUMELEMENTS(ring3)},
  {ring4, NUMELEMENTS(ring4)},
  {ring5, NUMELEMENTS(ring5)},
  {ring6, NUMELEMENTS(ring6)},
  {ring7, NUMELEMENTS(ring7)},
  {ring8, NUMELEMENTS(ring8)},
  {ring9, NUMELEMENTS(ring9)},
  {ring10, NUMELEMENTS(ring10)},
  {ring11, NUMELEMENTS(ring11)},
};



void setup(){
  PinReset(); 

}

unsigned long pulse_time = millis();
unsigned long timer = 5;

void loop() {
  for (uint16_t rowCnt = 0; rowCnt < NUMELEMENTS(ringMatrix); rowCnt++){
    for (int waiter = 0; waiter < floor(300/ringMatrix[rowCnt].numElements); waiter++){
    // while(millis() - pulse_time < timer){
      for (uint16_t colCnt = 0; colCnt < ringMatrix[rowCnt].numElements; colCnt++){
        LED_ON(ringMatrix[rowCnt].pins[colCnt]);
      }
    }
    pulse_time = millis();
  }
}


/////////////////////////// FUNCTIONS ///////////////////////////
void PinReset() {
  /* reset all pins to a count of pin_count
  */   
  for (int i = 0; i < pin_count; i++) {
    pinMode(pins[i], INPUT);
  }
}


int full_grid_brightness = 150; // optimal timing for full grid
int dimmer = 0;

void LED_ON(int LED){
    LED = LED - 1;

    pinMode(pins[LOW_POS[LED]], OUTPUT);
    pinMode(pins[HIGH_POS[LED]], OUTPUT);

    digitalWrite(pins[LOW_POS[LED]],LOW);
    digitalWrite(pins[HIGH_POS[LED]],HIGH);
    delayMicroseconds(full_grid_brightness);

    pinMode(pins[LOW_POS[LED]], INPUT);
    pinMode(pins[HIGH_POS[LED]], INPUT);    
    delayMicroseconds(dimmer);

}