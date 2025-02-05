//32-bit sine wave lookup table
// Vpp = 2.2V, period = 16.8ms
// const PROGMEM uint8_t sine[32] = {
//   128, 153, 177, 199, 218, 234, 245, 253,
//   255, 253, 245, 234, 218, 199, 177, 153,
//   128, 103,  79,  57,  38,  22,  11,   3,
//     0,   3,  11,  22,  38,  57,  79, 103
// };

// //32-bit inverted sine wave
// const PROGMEM uint8_t invertedSine[32] = {
//   128, 103,  79,  57,  38,  22,  11,   3,
//     0,   3,  11,  22,  38,  57,  79, 103,
//   128, 153, 177, 199, 218, 234, 245, 253,
//   255, 253, 245, 234, 218, 199, 177, 153
// };

// const PROGMEM uint8_t triangleWave[32] = { 
//     0, 16, 32, 48, 64, 80, 96, 112, 
//     128, 144, 160, 176, 192, 208, 224, 240, 
//     255, 240, 224, 208, 192, 176, 160, 144, 
//     128, 112, 96, 80, 64, 48, 32, 16 
// };

const PROGMEM uint8_t triangleWave_128[128] = { 
    0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 
    64, 68, 72, 76, 80, 84, 88, 92, 96, 100, 104, 108, 112, 116, 120, 124,
    128, 132, 136, 140, 144, 148, 152, 156, 160, 164, 168, 172, 176, 180, 184, 188,
    192, 196, 200, 204, 208, 212, 216, 220, 224, 228, 232, 236, 240, 244, 248, 252, 
    255, 252, 248, 244, 240, 236, 232, 228, 224, 220, 216, 212, 208, 204, 200, 196,
    192, 188, 184, 180, 176, 172, 168, 164, 160, 156, 152, 148, 144, 140, 136, 132,
    128, 124, 120, 116, 112, 108, 104, 100, 96, 92, 88, 84, 80, 76, 72, 68,
    64, 60, 56, 52, 48, 44, 40, 36, 32, 28, 24, 20, 16, 12, 8, 4
};

// const int sineSize = sizeof(sine) / sizeof(sine[0]);
// const int invertedSineSize = sizeof(invertedSine) / sizeof(invertedSine[0]);
// const int triangleSize = sizeof(triangleWave) / sizeof(triangleWave[0]);
const int triangleSize_128 = sizeof(triangleWave_128) / sizeof(triangleWave_128[0]);
const int DAC_0 = 25;
const int ADC_0 = 34;
// const int DAC_1 = ;

// 1 & 3 on when sine[i] > 128, off when < 128
// 2 & 4 on when sine[i] < 128, off when > 128
const int MOSFET1 = 13;
const int MOSFET2 = 14;
const int MOSFET3 = 26;
const int MOSFET4 = 27;

// void writeSine(){  
//     for (int i = 0; i < sineSize; i++){
//     dacWrite(DAC_0, sine[i]);
//     delayMicroseconds(521);
//   }
// }

// void writeInvertedSine(){
//     for (int i = 0; i < invertedSineSize; i++){
//     dacWrite(DAC_0, invertedSine[i]);
//     delayMicroseconds(521);
//   }
// }

uint8_t readSine(){
  uint8_t sineADC = analogRead(ADC_0);
  return (sineADC, 0, 4095, 0, 255); // 8bit
}

// void writeTriangle(){
//     for (int i = 0; i < triangleSize; i++){
//     dacWrite(DAC_0, triangleWave[i]);
//     delayMicroseconds(521);
//   }
// }

//writes 128 bit 8kHz triangle wave
void writeTriangle_128(){
    for (int i = 0; i < triangleSize_128; i++){
    dacWrite(DAC_0, triangleWave_128[i]);
    compare(triangleWave_128[i]);
    delayMicroseconds(125);
  }
}

void compare(uint8_t triangle_val){
  uint8_t sine_val = readSine();

  if(sine_val > triangle_val){
    digitalWrite(MOSFET1, HIGH);
    digitalWrite(MOSFET3, HIGH);
    digitalWrite(MOSFET2, LOW);
    digitalWrite(MOSFET4, LOW);
  } else{
    digitalWrite(MOSFET1, LOW);
    digitalWrite(MOSFET3, LOW);
    digitalWrite(MOSFET2, HIGH);
    digitalWrite(MOSFET4, HIGH);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(MOSFET1, OUTPUT);
  pinMode(MOSFET2, OUTPUT);
  pinMode(MOSFET3, OUTPUT);
  pinMode(MOSFET4, OUTPUT);
  pinMode(ADC_0, INPUT);
}

void loop() {
    //writeSine();
    writeTriangle_128();
  }

