
/*
Following define macro are managing behaviour of the application 

  This application demonstrate uses of voltage divider to identify the 
  unknow resistor know under name R2 from a know resistor R1 . 

 CAN_ADJUST_VOLTAGE -> Set to 1 or defined inside the application it
 should adjust the final voltage after getting the information of R2.
 This should proove something : 
  - It proove if the voltage is stable after its increasement, the resistor 
  should stay fixed and should have the same reading to a higher voltage. 

  - It also proove than nominal voltage should be higher than default voltage.
   As it's expressed in some experiment where they are reading external source 
   of voltage with commune ground does work with at least 100 KHoms from the 
   Analog PIN and 1 Mhoms from the ground of MCU and to following external 
   source. This activity proove the sensibility and it's measurement. With
   this type of activity, with applied divider on base of Analog pin, connecting 
   another pin and adding a level of voltage to reach DEF_CALIBRATION_VOLTAGE.
   Calibration voltage was choosed to to be higher than 3.3 but lower than 5v
    
  SERIAL_MSG -> Set to 1 or defined inside the application it
 should allow the compilation of the Serial Message being broadcasted by the 
 mcu to the usb port to your COMMUNICATION PORT . It does increase application 
 size and may exceed application limit. Some effort are made to reduce the 
 size of the application to offer some debug but limited by it's uses it's
 up to the option for viewing some information.  

  USING_SHIFT_REGISTER -> Set to 1 or defined inside the application it
 should allow  the uses of the voltage divider through an exposed apparatus 
 in an application with a register, it will get the voltage from one of the 
 registry pin (up to 8 by registers ) and this will get result from different 
 voltage divider setting and get the proper one. 

  USING_OLED_SSD1306 -> Set to 1 or defined inside the application it
 should allow the compilation Adafruit_GFX to handle SSD1306 in Adafruit fashion.
 It's a consuming library and may not fit entirely BasicOhmRead_AdaGFX.ino was 
 developped separetely to test and this version do include other alternative and  
 Adafruit_GFX as well 
 
  USING_SSD1306_U8GLIB -> Set to 1 or defined inside the application it
 should allow the compilation U8GLIB to handle SSD1306 on an economic way, 
 while essential procedure to show about result cost 9kb with U8GLIB while 
 costing up to 14kb, due its internal character map. 

  USING_SSD1306_U8GLIB2 -> Set to 1 or defined inside the application it
 should allow the compilation U8GLIB2 to handle SSD1306 on an economic way, 
 while essential procedure to show about result cost 9kb with U8GLIB while 
 costing up to 14kb. This library is added for testing difference between 
 U8GLIB and U8GLIB2 . 

*/


#define ANALOG_PIN              A0
#define ANALOG_VOLTAGE_PIN      A1
#define ANALOG_SAMPLING         10


//#define CAN_ADJUST_VOLTAGE      1
#define USING_SHIFT_REGISTER    1
#define USING_OLED_SSD1306      1


#define READ_SAMPLING           25
#define READ_DELAY              10
#define DEF_CALIBRATION_VOLTAGE 3.75 
#define DEF_REF_VOLTAGE         3.3
#define START_DELAY             2000 
#define END_DELAY               5000  
//#define NOT_SPI_DEVICE          1
//#define SERIAL_MSG              1


#ifdef USING_OLED_SSD1306
#define SCREEN_WIDTH            128 // OLED display width, in pixels
#define SCREEN_HEIGHT           32 // OLED display height, in pixels
#define OLED_RESET              -1
#define SCREEN_ADDRESS          0x3C
#endif 

#ifndef NOT_SPI_DEVICE
#include <SPI.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//#include "Wire.h"
#include <Adafruit_GFX.h>
#endif 

#define READING_RESISTOR_MSG           "VOUT:_VOL__U1_ _AW__S__AV__U4_\nR2: _RES_ _U2_\nMSG:_M_\nCOUNT(_U3_/c): _C_"

#ifdef USING_SHIFT_REGISTER
#include <ShiftRegister74HC595.h>

// create a global shift register object
// parameters: <number of shift registers> (data pin, clock pin, latch pin)
ShiftRegister74HC595<1> sr(4,5,6);
#endif 

/*
*********************** END OF DEFINE *********************** 
*/




#ifdef USING_OLED_SSD1306
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire /*, OLED_RESET*/); 
#endif 


String  StrMsg, StrErrMsg ; 
int     iAdjVoltScale = 0 , 
        analogPin = 0,
        raw = 0,
        SumRaw = 0 , 
        iCountStable, 
        iLevelConv ; 
//int     iArrayRead[READ_SAMPLING] ;  
char    *cArrayConv[] = { "K", "M", "G", "T" ,"P", "E" } ; 
char    *cArrayMsg[] = {  "NONE", 
                          "V.CAL. _V_V",
                          "A1 DOWN/DARK.NRG" ,
                          "LOW VOLT./HI.RES.",
                          "R2 IN RANGE R1",
                          "WET/NO READ",
                          "NO/CONTACT",
                          " OHMS",
                          "NOT READY",
                          "V",
                          "ADJ",
                          ":" } ; 
char    *cArrayRepl[] ={  "_VOL_", 
                        "_U1_",
                        "_AW_",
                        "_S_",
                        "_AV_",
                        "_U4_",
                        "_RES_",
                        "_U2_",
                        "_M_",
                        "_U3_",
                        "_C_",
                        "_V_" } ; 
long    lCount = 0 ; 
float   Vin = DEF_REF_VOLTAGE,
        VinAdj = 0.0 ,  
        Vout = 0,
        R1 =  259000,
        R2 = 0,
        buffer = 0;
bool    bErrorIsSet = false,
        bVAdjSet    = false ; 
/*
Previous R1 : 
  3560000 + 1588926  
  5148926 
  5148000
  5489000
  5830000
  6870000
  7000000
  ( 1690000 M ohms + diode ) 

  1Mohms + dioded circle on resistor side : 1470000 
  1Mohms + dioded circle on Analog Pin    : 1249000

  compiled size: 26902,26890,26898,27002,26946,27042,26946,
*/
void setup()
{
  Serial.begin(9600);

#ifdef USING_OLED_SSD1306
  if( ! display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) 
  {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
#endif

  /*
   * Section to configure the excess of voltage 
    Assuming we can plug a diode on Analog port 1 (A1) to make 
    it protected from flow-back after we do have an idea of the 
    resistor, adding the supplement of voltage for larger 
    resistor will simply add some voltage to make the further 
    reading being more precise. A set a default calculation to 
    opte to compensate to 3v3 voltage instead of 5 volts 
    directly.

    The idea come over a Wheatstone Bridge where they use 3 
    resistors and a second source to find the ideal or precise ohm 
    ratio for the fourth unknow resistor. 

    Calling creating a balanced network of resistor ; If I proove 
    the Arduino pin are already a setup of balanced diode because 
    GPIO are like NAND door with a latch and some extra like PULLING 
    them to treat the GPIO into an HIGH STATE. So it coming with a 
    partial proof of diode are a bit more than resistor but are 
    balanced throught the virtual ground of them . So there is 
    chance to get the supply working as same fashion than Wheatstone. 
  */
#ifdef CAN_ADJUST_VOLTAGE
#warning "CAN_ADJUST_VOLTAGE used and may overflow current, be sure you are using diode with A1"
  pinMode( ANALOG_VOLTAGE_PIN, OUTPUT );
  analogWrite(ANALOG_VOLTAGE_PIN, 0);
#endif

  delay(START_DELAY);

#ifdef USING_OLED_SSD1306
  //display.display() ; 
  display.clearDisplay();
  display.setTextSize(1);
#endif

 
#ifdef SERIAL_MSG
  Serial.println(F("OK"));
#endif 

}

void loop()
{
  bErrorIsSet = false ; 
   
  StrErrMsg   = cArrayMsg[0] ; 
  StrMsg      = READING_RESISTOR_MSG ; 
  lCount      = millis() / 1000 ; 
  
  
  iCountStable = 0 ;
  raw = 0 ; 
  SumRaw = 0 ; 
  while( iCountStable <= (READ_SAMPLING-1) )
  {
    raw = analogRead(analogPin);
    delay( READ_DELAY ) ; 
    //iArrayRead[iCountStable]=raw;
    iCountStable +=1 ;
#ifdef SERIAL_MSG
  Serial.println(String("Read no: ") + String( iCountStable ) + String(" Value : ") + String( raw ) );
#endif  
    SumRaw+=raw ; 
  }
  //for( int iC=0 ; iC <= (READ_SAMPLING-1) ; iC++)
  //{
  //  raw=raw+iArrayRead[iC] ;  
  //}
  raw=(SumRaw/READ_SAMPLING);
   
  if(raw )
  {
    //buffer = raw * Vin;
    Vout = (raw * Vin)/( pow(2,ANALOG_SAMPLING) );
    if( !bVAdjSet && ( Vout < DEF_CALIBRATION_VOLTAGE ) )
    {
      bErrorIsSet = true ;
      VinAdj = DEF_CALIBRATION_VOLTAGE - Vout ;
      iAdjVoltScale = VinAdj/(Vin/pow(2,8)) ;
//      StrErrMsg = "V.CAL. _V_V/_S_"  ;
      StrErrMsg = cArrayMsg[1]  ;
      StrErrMsg.replace( cArrayRepl[11] , String(VinAdj,2) ) ;  
//      StrErrMsg.replace(String("_S_"), String(iAdjVoltScale) ) ;
#ifdef SERIAL_MSG
  Serial.println( "Voltage is low, required ANALOG_VOLTAGE_PIN to push power : \n\t" + 
                  String( String(VinAdj,2)) + 
                  String("volts, or LEVEL:") + 
                  String(String(iAdjVoltScale)));
#endif
    }
    else
    {
      /* 
      Not required if default voltage is higher than default calibration 
      voltage ; But apparently with bigresistor it jump to 7.5 volts ? 
      */
      VinAdj =  0 ; 
      bErrorIsSet = true ;
      VinAdj = Vout - DEF_CALIBRATION_VOLTAGE  ;
      iAdjVoltScale = ( analogRead(analogPin) / pow(2,ANALOG_SAMPLING) ) * pow(2,8) ;
      //(pow(2,8)-1)-(VinAdj/(Vout/pow(2,8))) ;
//      StrErrMsg = "V.CAL. _V_V/_S_"  ;
      StrErrMsg = cArrayMsg[1]  ;
      StrErrMsg.replace( cArrayRepl[11],  String(VinAdj,2) ) ;  
//      StrErrMsg.replace(String("_S_"), String(iAdjVoltScale) ) ;
#ifdef CAN_ADJUST_VOLTAGE
#warning "CAN_ADJUST_VOLTAGE used and may overflow current, be sure you are using diode with A1."
    analogWrite(ANALOG_VOLTAGE_PIN, iAdjVoltScale);
    Vin-=VinAdj ; 
#endif
    }
    //buffer = (Vin/Vout) - 1;
    R2= R1 * ( (Vin/Vout) - 1 );
    //R2=Vout*R1/(Vin-Vout) ; 
  }
  /*
  Creating the compensation 
  After getting all then equation done we can push the Analog PIN A1 to the correct
  level with digitalwrite(A1,N) where N is iAdjVoltScale
  */
#ifdef CAN_ADJUST_VOLTAGE
#warning "CAN_ADJUST_VOLTAGE used and may overflow current, be sure you are using diode with A1."
  if( !bVAdjSet  )
  {
    /*Removing the bErrorIsSet priority after the voltage ischanged*/
    bErrorIsSet = false ; 
    analogWrite(ANALOG_VOLTAGE_PIN, iAdjVoltScale);
    Vin+= (Vin/pow(2,8))*iAdjVoltScale ; 
    bVAdjSet = true ; 
#ifdef SERIAL_MSG
    Serial.println("Vin updated to " + String(Vin) );
#endif
  }
#endif

#ifdef SERIAL_MSG
    Serial.print("Vout: ");
    Serial.println(Vout);
    Serial.print("R2: ");
    Serial.println(R2);
#endif 

  if( !bErrorIsSet && (Vout >= Vin) )
  {
    bErrorIsSet = true ;
    //StrMsg.replace(String(cArrayRepl[6]), String( cArrayMsg[8] ) ) ;  
    //StrMsg.replace(String(cArrayRepl[7]), String( "" )) ;
    StrMsg.replace( cArrayRepl[6] , cArrayMsg[8] ) ;  
    StrMsg.replace( cArrayRepl[7] , "" ) ;
    StrErrMsg = "CAP./DARK.NRG" ;
  }


  /*
  De-activate A1 in case voltage is stabilized and the R2 is negative. 
  Apparently a Dark energy case, while drawing at the resistor it sometimes
  turn into excess of voltage. 
  */
  if( !bErrorIsSet && ( R2 < -1000 ) )
  {
  
    bErrorIsSet = true ;
    StrMsg.replace( cArrayRepl[6], cArrayMsg[8] ) ;  
    StrMsg.replace( cArrayRepl[7], "" ) ;
    StrErrMsg = cArrayMsg[2] ;
  /* if CAN_ADJUST_VOLTAGE is not defined to something it does skip that 
  part and not touching to Vin. 
  */
#ifdef CAN_ADJUST_VOLTAGE
    analogWrite(A1, 0);
    //Vin-=VinAdj ;
    Vin = DEF_REF_VOLTAGE ;  
    bVAdjSet = false ; 
#endif
    //delay( READ_DELAY * 10 ) ; 

  }
  
  if( !bErrorIsSet && ( R2 < -10000 ) )
  {
  
    bErrorIsSet = true ;
    StrMsg.replace(cArrayRepl[6], cArrayMsg[8] ) ;  
    StrMsg.replace(cArrayRepl[7], "" ) ;
    StrErrMsg = cArrayMsg[2] ;
#ifdef CAN_ADJUST_VOLTAGE
    analogWrite(A1, 0);
    //Vin-=VinAdj ; 
    Vin = DEF_REF_VOLTAGE ;
    bVAdjSet = false ; 
#endif
  }  
  if( !bErrorIsSet && ( Vout < 1 )  )
  {
    StrErrMsg = cArrayMsg[3] ;
  }

/*
  Range algorithm is also the place where we can implement the 
  tolerancy in detecting range of ohmic value. According to
  definition of tolerancy it's accepting in percent the respect
  to th same value. This application do a basic 50% in range and 
  give message for NEAR where the reading value approach the 
  finite value and R2 IS GREATER after the 50% tolerancy of 
  the value is exceed, in term you play after until you 
  exceed the tolerancy. 
  To second level of test, replacing R2 > R1 by R2 > (TOL)
  and the other one R2 <= (TOL) and will match a value if 
  you say 5%, 15% tolerance in message from read . 
*/

  if( !bErrorIsSet && ( (R2 <= (R1-(R1/2))) || ( R2 <= (R1+(R1/2)) ) ) )
  {
    StrErrMsg = cArrayMsg[4] ; 
  }
  //else 
  //{
  //  
  //  if( ( R2 <= R1 ) && (R2 <= (R1+(R1/2)) ) ) 
  //  {
  //    StrErrMsg = "R2 NEAR R1" ;
  //  }
  //}

  //if( ( R2 < R1 ) )
  //{
  //  StrErrMsg = "R2 IS GREATER" ; 
  //}

  if( !bErrorIsSet && ( ( R2 < 0 ) && ( R2 > -1000 ) ) && !( Vout >= Vin) ) 
  {
    bErrorIsSet = true ;
    StrMsg.replace(cArrayRepl[6], cArrayMsg[8] ) ;  
    StrMsg.replace(cArrayRepl[7], "" ) ;
    StrErrMsg = cArrayMsg[5] ; 
  }

  if( R2 > 26000000 )
  {
    bErrorIsSet = true  ;
    StrErrMsg = cArrayMsg[6] ;  
#ifdef SERIAL_MSG
  Serial.println("\nMSG-STATE: " + StrErrMsg );
#endif 
  }
//  else
//  {
//   StrMsg.replace(String("_RES_"), String( R2 ) ) ;
//  }

  
  iLevelConv=round(log( R2 )/log(10) ) ; 
#ifdef SERIAL_MSG
  Serial.print("R2: ");
  Serial.println(R2);
  Serial.print("belong to base : ");
  Serial.println( iLevelConv );
  Serial.println("Final Format Prefix : " + cArrayConv[ iLevelConv-6 ] );
#endif 
  switch( iLevelConv )
  {
    //{ "k", "M", "G", "T" ,"P", "E" }
    case 5:
    case 6:
    case 7:
    if( R2 <= 2500000 )
    {
      StrMsg.replace(cArrayRepl[7], String( cArrayConv[ 0 ] ) + String ( cArrayMsg[7])   ) ;
      StrMsg.replace(cArrayRepl[6], String( R2/pow(10, (iLevelConv/2) ) ) ) ;
    }
    else
    {
      StrMsg.replace(String(cArrayRepl[7]), String( cArrayConv[ 1 ] ) + String( cArrayMsg[7] ) ) ;
      StrMsg.replace(String(cArrayRepl[6]), String( R2/pow(10, (iLevelConv) ) ) ) ;
    }
    //StrMsg.replace(String("_U2_"), String( cArrayConv[ 1 ] ) + String( " OHMS" ) ) ;
    //StrMsg.replace(String("_RES_"), String( R2/pow(10, (iLevelConv) ) ) ) ;
    case 8:
    case 9:
    case 10:
    StrMsg.replace(String(cArrayRepl[7]), String( cArrayConv[ 2] ) + String( cArrayMsg[7] ) ) ;
    StrMsg.replace(String(cArrayRepl[6]), String( R2/pow(10, (iLevelConv) ) ) ) ;   
    case 11:
    case 12:
    case 13:
    StrMsg.replace(String(cArrayRepl[7]), String( cArrayConv[ iLevelConv-6 ] ) + String( cArrayMsg[7] ) ) ;
    StrMsg.replace(String(cArrayRepl[6]), String( R2/pow(10,(iLevelConv-1)) ) ) ;
      break;
    default:
    StrMsg.replace(String(cArrayRepl[7]), String( cArrayMsg[7] ) );
    StrMsg.replace(String(cArrayRepl[6]), String( R2 ) ) ;
  }
  
  
  StrMsg.replace(String(cArrayRepl[0]), String( Vout ) ) ;
  StrMsg.replace(String(cArrayRepl[1]),cArrayMsg[9] ) ;

  if ( !bVAdjSet )
  {
   StrMsg.replace(cArrayRepl[2], "" ) ;
   StrMsg.replace(cArrayRepl[3], "" ) ;
   StrMsg.replace(cArrayRepl[4], "" ) ;
   StrMsg.replace(cArrayRepl[5], "" ) ;
  }
  else
  {
   StrMsg.replace(cArrayRepl[2], String( cArrayMsg[10] ) ) ;
   StrMsg.replace(cArrayRepl[3], String( cArrayMsg[11] ) ) ;
   StrMsg.replace(cArrayRepl[4], String(VinAdj,2) ) ;
   StrMsg.replace(cArrayRepl[5], String( cArrayMsg[9] ) ) ;
  }  
  
  if( lCount < 10000 )
  {
    StrMsg.replace(String(cArrayRepl[10]), String( lCount ) ) ;
    StrMsg.replace(String(cArrayRepl[9]), String( cArrayConv[ 0] ) ) ;  
  }
  else
  {
    StrMsg.replace(String(cArrayRepl[10]), String( lCount/1000 ) ) ;
    StrMsg.replace(String(cArrayRepl[9]), String( cArrayConv[ 1] ) ) ;
  }


  /*
  Message Replacement :
  This section will simply tell if the resistor acquire a stable value after N cycle
  or to report if the resistor is wet or having no contac at all. 
  */
  StrMsg.replace(String(cArrayRepl[8]), String( StrErrMsg ) ) ;

  

#ifdef USING_OLED_SSD1306  
  display.setCursor(0, 0);
  display.setTextColor(SSD1306_WHITE);
  display.print( StrMsg );
  display.display() ;
#endif 

#ifdef SERIAL_MSG
  Serial.println(F("DONE"));
#endif 
  delay(END_DELAY);

#ifdef USING_OLED_SSD1306
  display.clearDisplay();    
  
#endif


  
}
