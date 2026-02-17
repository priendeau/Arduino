#include <Arduino.h>
#include "SoftwareReset.hpp"
#include <ShiftRegisterPISO.h>

/* This work test the version 1.0.5 of ShiftRegisterPISO, outer work
 * proove the SoftwareReset from Giuseppe Masino does not interfer in
 * our work. It's possible to obtain equivalent result among nano328,
 * nano168, 32u4@16 Mhz, the pro-micro at 16Mhz and the lgt8f328@32 Mhz.
 * Switch to lower the calculation from 32 to 8 Mhz and 32 from 16 Mhz
 * are not yet added because main lgt8f328@32 Mhz remain displaying 0
 * as value and bit 0 to 7 being also showing 0. 
 *
 * The logic-IC used as shift register are NXP buy in bundle of 100 
 * samples and are soldered on a SSOP16 16 pin board are exactly
 * 74HC165, which the specification design stand in :
 * - maximum clock frequency : 56 Mhz.
 * - pin compatible with low power Schottky TTL (LSTTL)
 * - Are application fo Parallel-to-serial data conversion. 
 * - working voltage of wemos lgt8f328@32 Mhz is at 5.01 volts
 * and does not go lower in temperature than 25 deg. celsius.
 * As 74HC165 from Philips or NXP held a propagation between pin
 * and logical box of the shift register does not delay by more of 
 * 60ns normal condition are respected. 
 * 
 * this paragraph is long but inform of the property of schema held
 * many schema for mounting switch over 74HC165 and using several online
 * tips, some are inconsisten as some report nothing, it's the question :
 *  - Which design work ? 
 * - I have found 2 versions over www.makerguides.com and two setup 
 * Wire differently : look at the image:
 * 
 * - Initiative 1.
 * https://www.makerguides.com/more-inputs-with74hc1655-shift-register/
 * Pin from micro-controller to PISO Shift Register aka 74HC165 
 * Pin2 --> CLK 
 * Pin3 --> SH/LD
 * Pin4 --> QH
 * 
 * - Initiative 2.
 * https://www.makerguides.com/interfacing-parallel-in-serial-out-shift-register-74hc165-with-arduino/
 * Pin from micro-controller to PISO Shift Register aka 74HC165
 * Pin2 --> QH 
 * Pin3 --> CLK
 * Pin4 --> SH/LD
 * 
 *
 * Source information 
 * Used library Version 
 * - Arduino Internal
 * - SoftwareReset (Giuseppe Masino)
 * - ShiftRegisterPISO 1.0.5 (library reference : https://github.com/peto-3210/ShiftRegisterPISO/tree/master)
 *
 * Used platform Version
 * arduino:LGT8F328 2.0.7  
 *
 * Uses of original LGT8F328 red board TTGO-XI, a 328p equivalent. 
 * 
 * The work here : 
 * Reading switch state to have an answer which one is in use.
 * The micro-controller try 100 times and reboot. 
 * 
 * Since I work with ShiftRegisterPISO 1.0.5 it make realistic on how a 
 * basic frequency during operation is working within class PISORegister
 * which use micro() as timer and does apply a filter over the mome to 
 * read if it fall in the edge of width-pulse. This is why I add to my code
 * a method to ensure CLK from PISO register does own something similar at 
 * the CLK PIN or beside I do managing Initiative 2, and believe it almost 
 * a good schema. It lacking pull-up resistance and might rely on electric 
 * reference where they suggest 1k pull up resistance over 5v, where usually 
 * it's 10k.
 * - To this effort I add a clocking to the Pin 3 for LGT8F328 for 2 possible 
 * setting, 10Khz and 20Khz which is possible to monitor the frequency with a 
 * based Canadian Tire 052-0055-6 Digital Multimeter equiped of AT89C2051-24SU
 * Micro-controller. Or DSO-138 oscilloscope from DIY.
 *
 * - A profesionnal reference call Matlab from mathwork (MatLab Help center) 
 * where it's mathematic environment does badly uses the push-button I have 
 * on hand and do the example of using PORT-A from PISO Shift register to 
 * push button does put 5 volts and the Data-Port A and the pull-up resistor 
 * to one side of the push-button where other side is empty without connection. 
 * Usually I do claim to put the Data Port-A to left side of the push-button 
 * and pull-up and voltage to right side. Of course the pull-up resistor come 
 * from ground connection.
 * 
 * Website : https://www.mathworks.com/help/matlab/supportpkg/push-button-control-with-74hc165-shift-register.html
 * - And this webside show using 4 wire from micro-controller to PISO-Shift
 * Register.
 * 
 * Advice : 
 * PISO stand for : Parallel-In-Shift-Out --> 74HC165
 * SIPO stand for : Shit-In-Parallel-Out  --> 74HC595
 * */

/*
* Shifth Register 
  section:
*/
#define clkPin      3 //2
#define ldPin       4 //3
#define qhPin       2 //3
#define inputCount  8

const int clockPin = clkPin ;   // CLK
const int latchPin = ldPin  ;   // SH/LD
const int dataPin  = qhPin  ;   // QH

const int ledPin   = 12; // Led to start when micro-controller is working. 

const int numBits = inputCount ;
/*
* End of section
*/

/*
 * If you don't use arduino-cli, uncomment one of the following define :
 * SCALER_10KHZ - Use apparent internal configuration of LGT8F328 to compute
 *                a frequency from TCCR2A/TCCR2B
 * SCALER_20KHZ - Use apparent internal configuration of LGT8F328 to compute
 *                a frequency from TCCR2A/TCCR2B
 * SCALER_16KHZ - Use apparent internal configuration of LGT8F328 to compute
 *                a frequency from TCCR2A/TCCR2B
 *
 * SCALER_EXTERN - Default untouch FREQ_EXTERN_FREQ let external frequency
 *                 made from outside micro-controller to perform a frequency
 *                 and not touching FREQ_EXTERN_FREQ being set to 8000hz. 
 *                 By using FREQ_EXTERN_FREQ you can set yourself an 
 *                 electronic design to do the frequency you want. Using 
 *                 Timer 555 in astable mode I use R1=20K, R2=20k, C1=1.5nf
 *                 (assuming control_voltage in timer 555 is bind with 10nf 
 *                 at ground) to generate a frequency of 16Khz from pin3 of
 *                 timer 555.
 *  
 * If you are using arduino-cli, compiling with SCALER_EXTERN/FREQ_EXTERN_FREQ
 * define look like this:
 * arduino-cli compile --build-property "compiler.c.extra_flags=-DSCALER_EXTERN=1 -DFREQ_EXTERN_FREQ=16000" \
 *                     --build-property "compiler.c.extra_flags=-DSCALER_EXTERN=1 -DFREQ_EXTERN_FREQ=16000" \
 *                     --build-property "compiler.cpp.extra_flags=-DSCALER_EXTERN=1 -DFREQ_EXTERN_FREQ=16000" \
 *                     --jobs 12 --verbose --output-dir ./build/
 *  Or the corresponding frequency you want including frequency generator DIY, or arduino module like
 * sis5535 or ad9630 are external module working in i2c mode which require setting to be added. 
*/
//#define SCALER_10KHZ               1
//#define SCALER_20KHZ               1
//#define SCALER_16KHZ               1
//#define SCALER_EXTERN              1

#ifdef SCALER_10KHZ
#define FREQ_SHIFT_REG             10000
#endif

#ifdef SCALER_20KHZ
#define FREQ_SHIFT_REG             20000
#endif

#ifdef SCALER_16KHZ
#define FREQ_SHIFT_REG             16000
#endif

#ifdef SCALER_EXTERN
#define FREQ_SHIFT_REG             FREQ_EXTERN_FREQ
#endif

#ifndef FREQ_EXTERN_FREQ           8000
#endif

#if !defined( TERMINAL_BAUD )
#define TERMINAL_BAUD_SPEED         9600
#else
#define TERMINAL_BAUD_SPEED         TERMINAL_BAUD
#endif


#define MAJOR_VERSION               1
#define MINOR_VERSION               407

#define HOSTNAME                    "LGT8F328SRPiso"

#define DEFAULT_SLEEP               10000
#define IO_SLEEP                    15
#define TRIAL_COUNT                 100
#define ANTIGLITCH_READING          10

PISORegister r1;
int iReadNb = 0 ;
int iSerialReadVal = 0 ; 
bool bShitfRPisoRead = false ; 

void setup() 
{
  /*
   It's not only using the clockPin pin 3 to change the 
   state between LOW and HIGH, but require to use the 
   OC2B timer on pin3 to generate a clock source lower than
   32 Mhz.
  */
  #ifdef SCALER_20KHZ
  DDRD  |= (1 << clockPin);              // clockPin is PD3
  TCCR2A = (1 << WGM21) | (1 << COM2B0); // CTC Mode, Toggle OC2B
  TCCR2B = (1 << CS21);                  // Prescaler 8 (32MHz/8 = 4MHz)
  OCR2B = 99;                            // 4MHz / (2 * (1 + 99)) = 20kHz
  #endif
  #ifdef SCALER_16KHZ
  DDRD  |= (1 << clockPin);              // clockPin is PD3
  TCCR2A = (1 << WGM21) | (1 << COM2B0); // CTC Mode, Toggle OC2B
  TCCR2B = (1 << CS21);                  // Prescaler 8 (32MHz/8 = 4MHz)
  OCR2B = 124;                           // 4MHz / (2 * (1 + 124)) = 16kHz  
  #endif
  #ifdef SCALER_10KHZ
  DDRD  |= (1 << clockPin);              // clockPin is PD3
  TCCR2A = (1 << WGM21) | (1 << COM2B0); // CTC Mode, Toggle OC2B
  TCCR2B = (1 << CS11) | (1 << CS10);    // Prescaler 64 (32MHz/64 = 500Khz)
  OCR2B = 24;                            // 500Khz / (2 * (1 + 24)) = 10khz
  #endif
  /* 
    So final frequency is 10/20Khz, and require 
    to adjust the r1.SetFrequency to 10/20Khz.
  */

 /* Setting maximum current on all possible configurable 
  * pin.
  * In the choice of Pin 2, 3, 4, there is only Pin 2, The QH Pin for Piso
  * Serial register.
  * Which is able to set to the maximum of 80mA.
  * Belong to https://wolles-elektronikkiste.de/en/lgt8f328p-lqfp32-boards
  * we do have to deal with HDR register.  
  Table is here :
  HDR      Port/Pin     Pin Label (Nano)
  HDR0 --> PD5       --> D5
  HDR1 --> PD6       --> D6
  HDR2 --> PF1       --> TX
  HDR3 --> PF2       --> D2 
  HDR4 --> PE4 / PF4 --> none
  HDR5 --> PE5 / PF5 --> none 
 */
  
  HDR |= (1<<HDR3);             // Activate high current for Pin 2
  pinMode(dataPin,OUTPUT);      // Following instruction are required 
  digitalWrite(dataPin, HIGH);  // to "Clench-it" to the highest current.
  
  Serial.begin(TERMINAL_BAUD_SPEED);
  while ( !Serial )
  {
    delay(IO_SLEEP);
  }

  delay(DEFAULT_SLEEP);
  Serial.print("\r\n") ; 
  Serial.println( "BOOTING" );
  Serial.println("LGT8F328 Shift Register 74HC165");
  Serial.println( String( HOSTNAME ) + " Version " + String(MAJOR_VERSION) +"."+ String(MINOR_VERSION)+ "\r\n");
  Serial.println("Shift Register Frequency on Pin 3 should operate at: "+ String(FREQ_SHIFT_REG) + "hz");
#ifdef SCALER_EXTERN
  Serial.println("External frequency used to synchronize the PISO Shift Register\r\nExternal frequency configure set to "+ String(FREQ_EXTERN_FREQ) + "hz");
#endif
  Serial.println("\nWarning !!!\r\nAfter micro-controller work it reboot and start again.");
/*
* Shifth Register 
  section:
*/
  
  /* Apparently in this declaration of r1.Init
   *                 clkPol is True
   *                 ldPol  is False
   *                 inputLogic is True 
   *  - So dataPin/qhPin is not concer in this declaration.
  */
  r1.Init(numBits, clockPin, latchPin, dataPin, true, false, true);
  
  r1.SetReadingDelay(1000);
  r1.SetFrequency(FREQ_SHIFT_REG);
  r1.SetLdClkPulseDelay(50);
  
  pinMode(ledPin,INPUT);
  digitalWrite(ledPin, HIGH);
/*
* End of section
*/
  
}



void loop() 
{
  iReadNb++ ; 
  r1.SetGlitchPrevention( ANTIGLITCH_READING );
  r1.ReadData();
  iSerialReadVal = static_cast< int >( r1.GetAllInputData() ) ;
  Serial.println( "--------------------");
  Serial.println( "Trial number: " + String( iReadNb ) ); 
  Serial.println( "Start a reading sequence." );
  Serial.println( "Read number: " + String( iSerialReadVal ) ); 
  Serial.print( "Return value: " );
  for( int iC=0 ; iC < numBits; iC++)
  {
    bShitfRPisoRead = r1.GetInput( iC );
    if( bShitfRPisoRead == true )
    {
      Serial.print("1");
    }
    else
    {
      Serial.print("0");
    }
    
  }
  Serial.println();
  
  delay(DEFAULT_SLEEP/5) ; 
  if( iReadNb == TRIAL_COUNT )
  {
    Serial.println("\nPlease stand-by, micro-controller is rebooting after "+String( TRIAL_COUNT )+" trials.");
    digitalWrite(ledPin, LOW);
    delay(DEFAULT_SLEEP/5);
    softwareReset::standard() ;
  }
}

