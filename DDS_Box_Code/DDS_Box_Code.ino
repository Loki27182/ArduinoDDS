#include <SetListArduino.h>
#include <SPI.h>

#include "Teensy_FrequencyShield_2ch.h"

#include "AD9954.h"
#include "ADF4350.h"
//#include "ADF4107.h"

// Serial command library
// https://github.com/kroimon/Arduino-SerialCommand
//#include <SerialCommand.h>
//SerialCommand sCmd;



SetListArduino SetListImage(SETLIST_TRIG);


// Blue laser beatnote DDS reference
AD9954 beatnoteDDS(DDS1_CS, DDS1_RESET, DDS1_IOUPDATE, DDS1_PS0, 0, 0);

// Clock PLL
ADF4350 clock(PLL2_LE);

// Beatnote OPLL
//ADF4107 beatnotePLL(PLL1_LE);

SPISettings spi_settings(250000, MSBFIRST, SPI_MODE0);

void setup() {
  Serial.begin(115200);
  delay(1000);
  SPI.begin();
  delay(50);	// give it a sec
  SPI.beginTransaction(spi_settings);
  delay(100);
	
  clock.initialize(400, 10);      // init clock to output 400MHz, from 
				       // onboard 10MHz oscillator
  delay(10);
  clock.auxEnable(1);
  delay(10);
  clock.setAuxPower(3);
  delay(10);

// For AD9910 dds...
  beatnoteDDS.initialize(400000000);
  delay(100);
  
  // 88 beatnote at 1241.59MHz; / 25 = 49663600
  // 160 MHz bn @ 10x div = 17750000
 
  beatnoteDDS.setFreq(47000000);
  delay(100);
 
  //beatnotePLL.initialize(16,3,0,1);

  
  SetListImage.registerDevice(beatnoteDDS, 0);  
  
  SetListImage.registerCommand("f", 0, setFreq0);
  SetListImage.registerCommand("r", 0, setRamp);
 

}

void loop() {
   SetListImage.readSerial(); 
}

void setFreq0(AD9954 * dds, int * params){
  int freq = (int)params[0];
  dds->setFreq(freq);
}

void setRamp(AD9954 * dds, int * params){
  int f0 = params[0];
  int f1 = params[1];
  int tau = params[2];
    
  int redf0 = f0/1000;
  int redf1 = f1/1000;
    
  int RR = 200;
  int posDF = ((redf1-redf0)*RR + tau*100 - 1)/(tau*100);
  
  dds->linearSweep(f0,f1,posDF,RR,posDF,RR);
}
