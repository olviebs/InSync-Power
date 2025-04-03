#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include "config.h"

Adafruit_ADS1115 ads1115;	// Construct an ads1115 

const int DAC_0 = DAC0;

// 1 & 3 on when sine[i] > 128, off when < 128
// 2 & 4 on when sine[i] < 128, off when > 128
const int MOSFET1 = 13;
const int MOSFET2 = 14;
//const int MOSFET3 = 26;
//const int MOSFET4 = 27;


const uint8_t triangleWave_128[128] = { 
    0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 
    64, 68, 72, 76, 80, 84, 88, 92, 96, 100, 104, 108, 112, 116, 120, 124,
    128, 132, 136, 140, 144, 148, 152, 156, 160, 164, 168, 172, 176, 180, 184, 188,
    192, 196, 200, 204, 208, 212, 216, 220, 224, 228, 232, 236, 240, 244, 248, 252, 
    255, 252, 248, 244, 240, 236, 232, 228, 224, 220, 216, 212, 208, 204, 200, 196,
    192, 188, 184, 180, 176, 172, 168, 164, 160, 156, 152, 148, 144, 140, 136, 132,
    128, 124, 120, 116, 112, 108, 104, 100, 96, 92, 88, 84, 80, 76, 72, 68,
    64, 60, 56, 52, 48, 44, 40, 36, 32, 28, 24, 20, 16, 12, 8, 4
};

const uint8_t invertedTriangleWave_128[128] = { 
    255, 251, 247, 243, 239, 235, 231, 227, 223, 219, 215, 211, 207, 203, 199, 195, 
    191, 187, 183, 179, 175, 171, 167, 163, 159, 155, 151, 147, 143, 139, 135, 131,
    127, 123, 119, 115, 111, 107, 103, 99, 95, 91, 87, 83, 79, 75, 71, 67, 
    63, 59, 55, 51, 47, 43, 39, 35, 31, 27, 23, 19, 15, 11, 7, 3, 
    0, 3, 7, 11, 15, 19, 23, 27, 31, 35, 39, 43, 47, 51, 55, 59,
    63, 67, 71, 75, 79, 83, 87, 91, 95, 99, 103, 107, 111, 115, 119, 123,
    127, 131, 135, 139, 143, 147, 151, 155, 159, 163, 167, 171, 175, 179, 183, 187,
    191, 195, 199, 203, 207, 211, 215, 219, 223, 227, 231, 235, 239, 243, 247, 251
};

/*

1. Update carrier_period for lower period (7-10 us, currently at 125us)

2. Uncomment TC0_Handler and commented lines in setup() to try register manipulation 
    - Comment out compare() and write_compare()


*/

const int triangleSize_128 = sizeof(triangleWave_128) / sizeof(triangleWave_128[0]);
unsigned long previousTime = 0;
const int carrier_period = 125; 
int triangleIndex = 0;

/*

func. generator - 60Hz Sine @ 825mv offset, 1.65 Vpp

*/

uint16_t readSine(){
  int16_t adc0 = ads1115.readADC_SingleEnded(0);
  uint8_t sineADC = map(adc0, 0, 32767, 0, 255);
  return sineADC;
}

void compare(uint8_t sine_val, int8_t triangle_val){
  Serial.print("Sine: "); 
  Serial.print(sine_val);
  Serial.print("\tTriangle: "); 
  Serial.println((uint8_t)triangle_val);

  if(sine_val > triangle_val){
    digitalWrite(MOSFET1, HIGH);
    //digitalWrite(MOSFET3, HIGH);
    digitalWrite(MOSFET2, LOW);
   //digitalWrite(MOSFET4, LOW);
  } else{
    digitalWrite(MOSFET1, LOW);
    //digitalWrite(MOSFET3, LOW);
    digitalWrite(MOSFET2, HIGH);
   // digitalWrite(MOSFET4, HIGH);
  }
}

// void TC0_Handler(){
//   TC0->TC_CHANNEL[0].TC_SR;
//   uint8_t current_triangle = triangleWave_128[triangleIndex];
//   uint8_t sine_val = readSine();
//   analogWrite(DAC_0, current_triangle);
//   compare(sine_val, current_triangle);
//   triangleIndex = (triangleIndex + 1) % 128;
// }

void write_compare() {
    unsigned long currentTime = micros();
    if (currentTime - previousTime >= carrier_period) {
        previousTime = currentTime;
        uint8_t sine_val = readSine();
        analogWrite(DAC_0, invertedTriangleWave_128[triangleIndex]);
        compare(sine_val, invertedTriangleWave_128[triangleIndex]);
        triangleIndex = (triangleIndex + 1) % triangleSize_128;
    }
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  ads1115.begin();
  ads1115.setGain(GAIN_TWO);
  ads1115.setDataRate(RATE_ADS1115_860SPS);

  // pmc_set_writeprotect(false);
  // pmc_enable_periph_clk(ID_TC0);
  // TC_Configure(TC0, 0, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK1);

  // uint32_t stepRC = (42000000UL * carrier_period) / 1000000UL;

  // TC0->TC_CHANNEL[0].TC_IER = TC_IER_CPCS;
  // NVIC_EnableIRQ(TC0_IRQn);

  pinMode(MOSFET1, OUTPUT);
  pinMode(MOSFET2, OUTPUT);
}

void loop() {

  }

