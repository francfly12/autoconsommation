/* This version is stable and functional 
 * This is a very special version of Emon TX made by FSA
 * based on 
 * ARDUINO 101  card
 * Emon TX shield 
 * CT3 and T°sensor not used here, therefore pin A4 and A5 have been cut on the shield to enable to use pins for I2C display.
 * 2 pull up resistors added 4.7 K on pins A4 and A5. added one capacitor on 5V rail to stabilise supply
 * does not work on a classic arduino UNO due to insufficient memory
 * OLED display chinese version sh1106 driver
 * U8g2 library
 * EmonLib.h library
 * This circuit will display power and will be used for power diversion on a secondary circuit TBD
 * Open source software
 * contributions : francois SAHUC (FSA)
 * Robin Emley, Robert Wall, Trystan Lea
   Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)
   redistribution permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright notice
    and the following disclaimer.
  BEWARE OF ELECTRIC SHOCKS WHEN USING AND INSTALLING THOSE CIRCUITS
  AT YOUR OWN RISK
*/
// libraries
#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include "EmonLib.h"
// declarations
// Create  instances for each CT channel
EnergyMonitor ct1,ct2,ct3;
  // CT1  positive when exporting, from the grid, negative if we export
  // CT2  Positive when PV feeds power
  // CT3  Power used by a secondary circuit, just used for display
char bufferCT1 [16]; char bufferCT2 [16]; char bufferCT3[16];char bufferVrms [16]; // buffers to display values
long oldCT1,oldCT2,oldCT3,oldVrms; // 
long newCT1,newCT2,newCT3,newVrms ;  // used for filtering
long filter = 4; // low pass filter : 10 => value + 1/10 etc
int CT1i,CT2i,CT3i,Vrmsi; // because we need integer to transform and store inputs fron longintegers
const int LEDpin = 9; // On-board emonTx LED    
int triac_Pin = 6; // Pin Used to transmit triac modulated orders, will be decoded by another circuit.
int Red_Led_pin = 10; // Red LED , or the red pin of a tricolor LED,
                      //  red LED , Flickers to show activity (heartbeat once a second)
int Blue_Led_pin = 12; // Blue LED , shows Solar power production
      // Green Led is directly connected on the triac output (pin 6) to show PWM

float Duty_Cycle = 0 ; // from 0 to 100 %, used for order computation
int Duty_Cyc; // same, but as an integer
int triac_duty = 0 ; // from 0 to 255 ,used for the analogwrite function
    // adapt the following to your installation :
float MaxDump = 2200 ; // used to compute % of power diverted
                    // i.e. a water heater of 2200 W dumps 100% of the load
int MaxConsumption = 9000 ; // max power rated for the total installation, used to show red led 
int MaxSolar = 300 ; // maximum power of your solar arrays, used to show blue
int Min_Solar = 10; // used to setup minimum for Blue LED
float P1,P2;          
  // P1 = total power consumed by the system, input from the grid, measured by CT1
  // P2 = solar power
int P1i,P2i; // same in integer format  
float P1target = 10 ; // P1target = what amount of power we want to consume from the grid in Watts 
float Pc ; // Power commanded for diversion
int Install_Max_Pwr ; // used to put on a red LED 
// constructor for Oled display
 U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

void u8g2_prepare(void) {
  // see https://github.com/olikraus/u8g2/wiki/fntlistall
  u8g2.setFont(u8g2_font_8x13_t_symbols);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);

  u8g2.setFont(u8g2_font_unifont_t_symbols);
  
}

uint8_t draw_state = 0;

void setup(void) {

   // Setup indicator LED
  pinMode(LEDpin, OUTPUT); pinMode(Red_Led_pin,OUTPUT); pinMode(triac_Pin,OUTPUT); pinMode(Blue_Led_pin,OUTPUT);                                              
  digitalWrite(LEDpin, HIGH);  
   // brief test Led at start, 
  for (int x=0; x<6 ; x++) {
  digitalWrite(LEDpin, HIGH);
  digitalWrite(Red_Led_pin,HIGH); 
  delay (50);
  digitalWrite(Red_Led_pin,LOW); 
  digitalWrite(triac_Pin,HIGH); 
  delay (50);
  digitalWrite(triac_Pin,LOW); 
  digitalWrite(Blue_Led_pin,HIGH); 
  delay (50);
  digitalWrite(Blue_Led_pin,LOW); 
  digitalWrite(LEDpin, LOW);
    
  }

  pinMode (triac_Pin,OUTPUT) ;  // set output
    
  u8g2.begin();

delay(500); // pause half a second to stabilise readings at startup
/*Serial.begin(57600);  // for serial port display, this part can be deleted for final use
   // wait for the Serial port to connect. Open the Serial Monitor to continue executing the sketch
  //while(!Serial);
  Serial.println(" ");
  Serial.println("FSA_release_1.4"); 
  Serial.println("OpenEnergyMonitor.org");
  */
  
  // Calibration factor = CT ratio / burden resistance = (100A / 0.05A) / + some tricks for adjustments
  ct1.current(1, 92);// these are adjusted using a wattmeter
  ct2.current(2, 89.7);                                     
  ct3.current(3, 89.9);
    
  // (ADC input, calibration, phase_shift)
  ct1.voltage(0, 265, 1.7);  //  adjusted using a voltmeter
  ct2.voltage(0, 265, 1.7);                                
  ct3.voltage(0, 265, 1.7); 

}

void loop(void) {
  // make led flash each loop for check
  digitalWrite(LEDpin, HIGH); // red light on the EmonTX shield 
     // Red Led shows heartbeat once per second      
  // Blu led if PV producing
    if (P2i>Min_Solar) { digitalWrite (Blue_Led_pin,HIGH); };
       delay (2);
    if (P2i>(MaxSolar/4)) { delay (2); };  // longer flash for more power
     if (P2i>(MaxSolar/2)) { delay (4); };
      if (P2i>(MaxSolar*0.75)) { delay (8); };
     digitalWrite (Blue_Led_pin,LOW);
     digitalWrite (Red_Led_pin,HIGH); 
     delay (1); // minimum flash   
    if (P1i>P2i) { delay(2);  }; // minimum flash
              if (P1i > (MaxConsumption / 4)) { delay (2); };
       if (P1i > (MaxConsumption * 0.5)) { delay (4); };
       if (P1i > (MaxConsumption * 0.75)) { delay (8); };
      digitalWrite(Red_Led_pin,LOW);
   digitalWrite(LEDpin, LOW);
  // green Led is connected on PWM

  
   
// Calculate all. No.of crossings, time-out 
  ct1.calcVI(20,1000);                                                  
  ct2.calcVI(20,1000);
  ct3.calcVI(20,1000);
  // calcul
  newCT1 = ct1.realPower;
  newCT2 = ct2.realPower;
  newCT3 = ct3.realPower;
  newVrms = (ct1.Vrms);
  // filtering
  newCT1= oldCT1 + ((newCT1-oldCT1)/filter);
  newCT2= oldCT2 + ((newCT2-oldCT2)/filter);
  newCT3= oldCT3 + ((newCT3-oldCT3)/filter);
  newVrms= oldVrms + ((newVrms-oldVrms)/filter);

  // transform to integers
  CT1i=int (newCT1);
  CT2i=int (newCT2);
  CT3i=int (newCT3);
  Vrmsi=int (newVrms);
  // and renew
  oldCT1 = newCT1;
  oldCT2 = newCT2;
  oldCT3 = newCT3;
  oldVrms = newVrms;
  transmit () ;
  
 // Available properties: ct1.realPower, ct1.apparentPower, ct1.powerFactor, ct1.Irms and ct1.Vrms
 // now display on OLED same values
 // construct the four lines
  sprintf(bufferCT1,  "Conso   %4d W",CT1i);
  sprintf(bufferCT2,  "Solaire %4d W",CT2i);
  sprintf(bufferCT3,  "Zone 2  %4d W",CT3i);
  sprintf(bufferVrms, "%3d V      %2d%%",Vrmsi,Duty_Cyc);

  // compute commands for triac
  // it is a % of load to divert excess solar power to the water heater
   
  P1i = int (newCT1); //  positive when exporting, from the grid, negative if we export
  P2i = int (newCT2); //  positive when PV produces
  // the target is to reduce power exhaustion to the network as much as possible
  // We want to reach P1target. for this, we command the Duty_Cycle
       Duty_Cycle= Duty_Cycle + ((-P1i/MaxDump)*100);
       // put some conditions here if there is hysteresis
      if (Duty_Cycle < 0) { Duty_Cycle = 0 ;};
      if (Duty_Cycle > 100) { Duty_Cycle = 100 ;};
  Duty_Cyc = int (Duty_Cycle); // used because an integer is needed for display  
  triac_duty = map (Duty_Cycle,0,100,0,255); // transforms into a command 0/255
  analogWrite (triac_Pin, triac_duty);  // et voila, sending order!
                                      // this signal is a PWM, not fit for a triac
                                      // this order has to be treated by another circuit
 
  
   
  // picture loop  
  u8g2.clearBuffer();
  draw(draw_state);
  u8g2.sendBuffer();
  
  // increase the state, used to animate display
  draw_state++;
  if ( draw_state >= 8 )
    {draw_state = 0;};

  // delay between each page
  delay(100);
}
