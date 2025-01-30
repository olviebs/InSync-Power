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
const int invertedSineSize = sizeof(invertedSine) / sizeof(invertedSine[0]);
const int DAC_1 = 25;
// const int DAC2 = ;

// 1 & 3 on when sine[i] > 128, off when < 128
// 2 & 4 on when sine[i] < 128, off when > 128
const int MOSFET1 = 13;
const int MOSFET2 = 14;
const int MOSFET3 = 26;
const int MOSFET4 = 27;

void writeSine(){  
    for (int i = 0; i < sineSize; i++){
    dacWrite(DAC_1, sine[i]);
    delayMicroseconds(521);
  }
}

void writeInvertedSine(){
    for (int i = 0; i < invertedSineSize; i++){
    dacWrite(DAC_1, invertedSine[i]); // make dac2
    delayMicroseconds(521);
  }
}

void MOSFETControl(uint8_t sine_value, uint8_t inverted_value){
  if(sine_value > 128){
    digitalWrite(MOSFET1, HIGH);
    digitalWrite(MOSFET2, HIGH);
  } else {
    digitalWrite(MOSFET1, LOW);
    digitalWrite(MOSFET2, LOW);
  }

  if(inverted_value > 128){
    digitalWrite(MOSFET3, HIGH);
    digitalWrite(MOSFET4, HIGH);
  } else {
    digitalWrite(MOSFET3, LOW);
    digitalWrite(MOSFET4, LOW);
  }
}

void setup() {
  pinMode(MOSFET1, OUTPUT);
  pinMode(MOSFET2, OUTPUT);
  pinMode(MOSFET3, OUTPUT);
  pinMode(MOSFET4, OUTPUT);
}

void loop() {
    writeSine();
    //writeInvertedSine();
  }
