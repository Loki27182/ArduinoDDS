// Beatnote box with digital PLL -- testing!!
#include <SetListArduino.h>
#include <SPI.h>

#include "Teensy_FrequencyShield_2ch.h"

#include "ADF4350.h"
#include "AD9954.h"
//#include "ADF4107.h"

#define DDS_REF_FREQ 400.0  // DDS reference frequency

SetListArduino SetListImage(SETLIST_TRIG);

ADF4350 clock(PLL2_LE);

// should refactor/update DDS library; PS1, OSK not used or connected --
// what should be desired behavior?
AD9954 refDDS(DDS1_CS, DDS1_RESET, DDS1_IOUPDATE, DDS1_PS0, 0, 0); 

//AD9954 refDDS(DDS2_CS, DDS2_RESET, DDS2_IOUPDATE, DDS2_PSO, 0, 0)

// what should be done about MUX input?? 
// Also refactor PLL libraries to accomodate this.
ADF4107 beatnotePLL(PLL1_LE);

SPISettings spi_settings(250000, MSBFIRST, SPI_MODE0);

void setup(){
  Serial.begin(115200);
  SPI.begin();
 // SPI.setClockDivider(6);
  //SPI.setDataMode(SPI_MODE0);
  SPI.beginTransaction(spi_settings);
  delay(100);
  
  clock.initialize(400,10);
  
  // enable auxiliary outputs
  clock.auxEnable(1);
  clock.setAuxPower(3);
   
  delay(500);
  
  refDDS.initialize(400000000);
  
  delay(500);

  //DDS1.setFreq(16000000);
  //refDDS.setFreq(58526000);
 // refDDS.setFreq(58534000); //87 beatnote is 1463.15MHz/25 = 58.526
// refDDS.setFreq(47753000); //88 beatnote is 1241.59MHz/26 = 47.753
  //refDDS.setFreq(43103000); //86 beatnote is = 1077.578MHz/25 = 43.103 

beatnotePLL.initialize(8,4,2,1);

  SetListImage.registerDevice(refDDS, 1);

SetListImage.registerCommand("f", 0, setFreq);
SetListImage.registerCommand("r", 0, setRamp);

//#if ISOTOPE == 88
//  refDDS.setFreq(47753000);
//  beatnotePLL.initialize(8,3,2,1); //88 setting
//#elif ISOTOPE == 87
//  refDDS.setFreq(58534000);
//  beatnotePLL.initialize(8,3,1,1);
//#elif ISOTOPE == 86
  //BN: 88-163.8174MHz
  //BN at 1077.7606/25=43.110424
//  refDDS.setFreq(43110424);
//  beatnotePLL.initialize(8,3,1,1);
//#elif ISOTOPE == 84
//refDDS.setFreq(35604000); //84 beatnote: try 1241.59-351.5 = 890.09/25 = 35.604 MHz
//  beatnotePLL.initialize(8,3,1,1);
//#endif
  
 // refDDS.setFreq(35604000); //84 beatnote: try 1241.59-351.5 = 890.09/25 = 35.604 MHz
// refDDS.setFreq(60000000); 
 // refDDS.setFreq(29264000);
  delay(500);
 //beatnotePLL.initialize(8,3,1,2);
  // P, B, A, R
  // beatnote = [(P*B+A)/R]*DDS Ref Freq.
  // Thus, DPLL.initialize(8,3,1,1) gives a 25x multiplier to DDS frequency.
// beatnotePLL.initialize(8,3,1,1); //87, 86, and 84 setting
//beatnotePLL.initialize(8,3,2,1); //88 setting

//refDDS.setFreq(15000000);
//delay(500);
//beatnotePLL.initialize(8,3,1,1);
//  sCmd.addCommand("dds", setDDS);
//   sCmd.addCommand("pll", setPLL);
//   sCmd.addCommand("l",setT);
//   sCmd.setDefaultHandler(unrecognized);    
   
}


void setT(){
  beatnotePLL.initialize(8,3,1,1);
}

void loop(){
 // sCmd.readSerial();
    SetListImage.readSerial(); 

}

void setRamp(AD9954 * dds, int * params){
  int f0 = params[0];
  int f1 = params[1];
  int tau = params[2];
    
  int redf0 = f0/1000;
  int redf1 = f1/1000;
    
  int RR = 200;
  int posDF = ((redf1-redf0)*RR)/(tau*100);
  
  dds->linearSweep(f0,f1,posDF,RR,posDF,RR);
}

void setDDS(){
  char * arg;
  arg = sCmd.next();
  int freq;
  if (arg != NULL) {
   freq = atoi(arg);  
   refDDS.setFreq(freq); 
   Serial.print("Set DDS frequency: ");
   Serial.println(freq);
  }else {
    Serial.println("error setting dds");
  }
}

void setPLL(){
 char * arg;
 int p;
 int b; 
int a;
int r;
arg = sCmd.next();
p = atoi(arg);
arg = sCmd.next();
b = atoi(arg);
arg = sCmd.next();
a = atoi(arg);
arg = sCmd.next();
r = atoi(arg);

if (p != 0 && b != 0 && a != 0 && r != 0){
  beatnotePLL.initialize(p,b,a,r);
  Serial.print("set pll: p = ");
  Serial.print(p);
  Serial.print("; b = ");
  Serial.print(b);
  Serial.print("; a = ");
  Serial.print(a);
  Serial.print("; r = ");
  Serial.println(r);
}else {
  Serial.println("error setting pll");
} 
}

void unrecognized(const char *command) {
  Serial.print("What is ");
  Serial.print(command);
  Serial.println("?");
  Serial.println("Valid commands: 'dds <FREQ>' and 'pll <P> <B> <A> <R>'");
}
