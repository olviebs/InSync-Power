//32-bit sine wave lookup table
// Vpp = 2.2V, period = 16.8ms
const PROGMEM uint8_t sine[32] = {
  128, 153, 177, 199, 218, 234, 245, 253,
  255, 253, 245, 234, 218, 199, 177, 153,
  128, 103,  79,  57,  38,  22,  11,   3,
    0,   3,  11,  22,  38,  57,  79, 103
};

//32-bit inverted sine wave
const PROGMEM uint8_t invertedSine[32] = {
  128, 103,  79,  57,  38,  22,  11,   3,
    0,   3,  11,  22,  38,  57,  79, 103,
  128, 153, 177, 199, 218, 234, 245, 253,
  255, 253, 245, 234, 218, 199, 177, 153
};

const int sineSize = sizeof(sine) / sizeof(sine[0]);
const int dac_pin = 25;
const int mosfet1_pin = 13;
const int mosfet2_pin = 14;
const int mosfet3_pin = 26;
const int mosfet4_pin = 27;

void writeSine(){  
    for (int i = 0; i < sineSize; i++){
    dacWrite(dac_pin, sine[i]);
    delayMicroseconds(521);
  }
}

void writeInvertedSine(){
    for (int i = 0; i < sineSize; i++){
    dacWrite(dac_pin, invertedSine[i]);
    delayMicroseconds(521);
  }
}

void setup() {
  pinMode(mosfet1_pin, OUTPUT);
  pinMode(mosfet2_pin, OUTPUT);
  pinMode(mosfet3_pin, OUTPUT);
  pinMode(mosfet4_pin, OUTPUT);
}

void loop() {
    writeSine();
  }

