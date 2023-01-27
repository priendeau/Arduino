
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
//#define CAN_ADJUST_VOLTAGE      1
//#define SERIAL_MSG              1
#define USING_SHIFT_REGISTER    1
//#define USING_OLED_SSD1306      1
//#define USING_SSD1306_U8GLIB2   1
#define USING_SSD1306_U8GLIB    1


#if defined( __AVR_ATmega328__ )
#define ANALOG_SAMPLING         10
#endif
#if defined( __AVR_ATmega32U4__ )  
#define ANALOG_SAMPLING         10
#endif
#if defined(__LGT8FX8P__)
#define ANALOG_SAMPLING         12
#endif


#ifdef USING_SSD1306_U8GLIB
#define U8GLIB_FONTNAME         u8g_font_fixed_v0
#define CHARSET_HEIGHT          8 
#endif 


#define READ_SAMPLING           25
#define READ_DELAY              10
#define DEF_CALIBRATION_VOLTAGE 3.75 
#define DEF_REF_VOLTAGE         3.3
#define START_DELAY             2000 
#define END_DELAY               5000  
//#define NOT_SPI_DEVICE          1
//#define SERIAL_MSG              1

/*
*********************** END OF DEFINE *********************** 
*/


#ifdef USING_SHIFT_REGISTER
#include <ShiftRegister74HC595.h>

// create a global shift register object
// parameters: <number of shift registers> (data pin, clock pin, latch pin)
ShiftRegister74HC595<1> sr(4,5,6);
#endif 

#ifdef USING_SSD1306_U8GLIB
#include "U8glib.h"
//#include "U8x8lib.h"
U8GLIB_SSD1306_128X32 u8g(U8G_I2C_OPT_NONE) ;  // I2C / TWI 
#endif 

enum  enum_FunctionReturnType {   asChar  = 0 , 
                                  asInt   = 1 , 
                                  asLong  = 2 }  ;

enum enum_TagDef { 
                  VolNumCol1e1      = 0,
                  VolUnitCol1e2     = 1,
                  VolWordAdjCol1e3  = 2,
                  VolSepCol1e4      = 3,
                  VolNumCol1e5      = 4, 
                  VolUnitCol1e6     = 5,
                  ResNumCol2e1      = 6, 
                  ResUnitCol2e2     = 7,
                  MSGWordCol3e1     = 8, 
                  CountMetricCol4e1 = 9, 
                  CountNumCol4e2    =10 } ;

typedef enum enum_TagDef eTagDef ; 

struct st_ColumnTagDef { char *cStrTag ; 
                        eTagDef eTagOrder ;
                        void *varRef ;  
                       } ; 
typedef struct st_ColumnTagDef stColDisplayRank ; 
                      

typedef enum_FunctionReturnType eFRType ; 

bool    bErrorIsSet = false,
        bVAdjSet    = false ; 
int     iAdjVoltScale = 0 , 
        analogPin = ANALOG_PIN,
        raw = 0,
        SumRaw = 0 , 
        iCountStable, 
        iLevelConv ;
        
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

                          
stColDisplayRank cArrayRepl[] ={  {"_VOL_", VolNumCol1e1      , NULL },
                                  {"_U1_",  VolUnitCol1e2     , NULL },
                                  {"_AW_",  VolWordAdjCol1e3  , NULL },
                                  {"_S_",   VolSepCol1e4      , NULL }, 
                                  {"_AV_",  VolNumCol1e5      , NULL },
                                  {"_U4_",  VolUnitCol1e6     , NULL },
                                  {"_RES_", ResNumCol2e1      , NULL },
                                  {"_U2_",  ResUnitCol2e2     , NULL },
                                  {"_M_",   MSGWordCol3e1     , NULL },
                                  {"_U3_",  CountMetricCol4e1 , NULL },
                                  {"_C_",   CountNumCol4e2    , NULL } } ;

                  
                  
char *cStrTpl[4] = { "VOUT:_VOL__U1_ _AW__S__AV__U4_", 
                     "R2: _RES_ _U2_",
                     "MSG:_M_",
                     "COUNT(_U3_/c): _C_" } ; 
char *cStrColMsg = NULL ; 

long    lCount = 0 ; 
float   Vin = DEF_REF_VOLTAGE,
        VinAdj = 0.0 ,  
        Vout = 0,
        R1 =  259000,
        R2 = 0,
        buffer = 0;

  /*
  Other font tested :
  u8g_font_5x8          - tinyest can get 5 lines of information 
  u8g_font_osb21        -
  u8g_font_fub11        - too big 
  u8g_font_gdr9         - a bit big  - 2 lines informations 
  u8g_font_unifont_0_8  - bit big - 2 lines informations 
  u8g_font_04b_03b      - tiny can get 6 lines of informations 
  u8g_font_04b_24       - tiny 
  u8g_font_unifont_4_5  - Indies
  u8g_font_unifont_0_8  - bit big - 2 lines informations 
  u8g_font_fixed_v0     - tiny, bit bigger than u8g_font_5x8 
  */  


#ifdef USING_SSD1306_U8GLIB
//u8g_fntpgm_uint8_t fontName
void draw(U8GLIB_SSD1306_128X32 &U8_SSDInterface, char *cArray[]) 
{
  char *cStrLineDisplay ; 
  // graphic commands to redraw the complete screen should be placed here  
  //u8g.setFont(u8g_font_unifont);
  U8_SSDInterface.setFont( U8GLIB_FONTNAME );
  for( int iC =0 ; iC <= sizeof(cArray)+1; iC++ )
  {
    cStrLineDisplay = static_cast< char *>( malloc( sizeof(cArray[iC]) ) ) ;
    cStrLineDisplay = cArray[iC] ; 
    updateValue( &cStrLineDisplay ) ; 
    u8g.drawStr( 0, (iC*CHARSET_HEIGHT)+CHARSET_HEIGHT, cStrLineDisplay );
    
  }


}
#endif

void updateValue( char **cStrCol)
{
  
}


#ifdef USING_SSD1306_U8GLIB
void setupU8GMode( U8GLIB_SSD1306_128X32 &U8_SSDInterface )
{
  if ( U8_SSDInterface.getMode() == U8G_MODE_R3G3B2 ) 
  {
    U8_SSDInterface.setColorIndex(255);     // white
  }
  else if ( U8_SSDInterface.getMode() == U8G_MODE_GRAY2BIT ) 
  {
    U8_SSDInterface.setColorIndex(3);         // max intensity
  }
  else if ( U8_SSDInterface.getMode() == U8G_MODE_BW ) 
  {
    U8_SSDInterface.setColorIndex(1);         // pixel on
  }
  else if ( U8_SSDInterface.getMode() == U8G_MODE_HICOLOR ) 
  {
    U8_SSDInterface.setHiColorByRGB(255,255,255);
  }
}
#endif


#ifdef USING_SSD1306_U8GLIB
//, u8g_fntpgm_uint8_t fontName 
void updateU8GDraw( U8GLIB_SSD1306_128X32 &U8_SSDInterface ) 
{
  U8_SSDInterface.firstPage();  
  do 
  {
    draw( U8_SSDInterface, cStrTpl );
  } while( U8_SSDInterface.nextPage() );  
}
#endif 

// , eFRType returnType 
void SearchReplValueChar( char &cStrColLine,  char *cStrTagSearch, char *cStrRepl  )
{
  //cStrColMsg
  char *chIndexSearch = NULL ;
  char *cStrSearchTxt = NULL ; 
  chIndexSearch=strstr( cStrColLine , cStrTagSearch );
  if (chIndexSearch != NULL)
  {
    strncpy(chIndexSearch, cStrRepl ,sizeof( cStrRepl ) );
  }
  
}


int ReadResistor( int* iResPin, long lDelay, int iSampling  )
{
  while( iCountStable <= (iSampling-1) )
  {
    raw = analogRead(iResPin);
    delay( lDelay ) ; 
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
  return raw ;
}


void ConvertResistorValue( void )
{
  if(raw )
  {
    Vout = (raw * Vin)/( pow(2,ANALOG_SAMPLING) );
    VinAdj = DEF_CALIBRATION_VOLTAGE - Vout ;
    iAdjVoltScale = VinAdj/(Vin/pow(2,8)) ; 
    
  }
  R2=Vout*R1/(Vin-Vout) ; 

}

void setup(void) 
{
  // assign default color value
#ifdef USING_SSD1306_U8GLIB
  setupU8GMode( u8g )  ; 
#endif

  /*
    Configuration of location where in the feed the variables are binded here by 
    reference and the uses of void* was to allow a simple memory affection from 
    varRef so we can work with the Variable directly and it's already 
    binded on the cArrayRepl so the display will loop for cArrayRepl and extracting
    location by reference for displaying Vout, Vin, R2 ...
  */

  cArrayRepl[VolNumCol1e1].varRef=&Vout ; 
  cArrayRepl[VolUnitCol1e2].varRef=&cArrayMsg[9];
  cArrayRepl[VolWordAdjCol1e3].varRef=&cArrayMsg[10];
  cArrayRepl[VolSepCol1e4].varRef=&cArrayMsg[11];
  cArrayRepl[VolNumCol1e5].varRef=&VinAdj;
  cArrayRepl[VolUnitCol1e6].varRef=&cArrayMsg[9];
  //cArrayRepl[ResNumCol2e1].varRef=&;
  //cArrayRepl[ResUnitCol2e2].varRef=&;
  //cArrayRepl[MSGWordCol3e1].varRef=&;
  //cArrayRepl[CountMetricCol4e1].varRef=&;
  //cArrayRepl[CountNumCol4e2].varRef=&;

}

void loop(void) 
{

  bErrorIsSet = false ; 
  lCount      = millis() / 1000 ; 
  iCountStable = 0 ;
  raw = 0 ; 
  SumRaw = 0 ; 

  raw=ReadResistor( analogPin,
                    READ_DELAY,
                    READ_SAMPLING) ;
  ConvertResistorValue( ) ;  


// picture loop
#ifdef USING_SSD1306_U8GLIB
  updateU8GDraw( u8g ) ;
#endif 

  // rebuild the picture after some delay
  delay(END_DELAY);
}
