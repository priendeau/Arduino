#include "U8glib.h"

#include <Stepper.h>

//#define USE_SERIAL_MSG              1

#define MAXSPEEDRPM             12
#define TIMESPINMOTOR           60000   // Motor delay, 60 seconds
#define BOOTDELAY               2500    // Boot delay 2.5 sec
#define PAGEDELAY               1500     // Between Page delay 1.5 seconds .

#define USE_MOTOR_WIRING1       1
#define MAXSPEEDRPM             12

//#define USE_MOTOR_WIRING2       1

//#define USE_SERIAL_MSG              1


#define CHARSET_HEIGHT          8
//#define U8G_FONT_NAME           u8g_font_unifont
#define U8G_FONT_NAME           u8g_font_fixed_v0
#define PIXELXBORDER            0
#define PIXELYBORDER            8

#define MAXPAGE                 4

//|U8G_I2C_OPT_DEV_0

U8GLIB_SSD1306_128X32 u8g(U8G_I2C_OPT_NONE);  // I2C / TWI 

const int stepsPerRevolution =  2038;

#if defined( USE_MOTOR_WIRING1 )
/* There is apparently 2 schemas for wiring, but don't tell me there is slow wiring for 
getting slower or the ordinary wiring for normal operation, when it does not exess 20rpm...*/
Stepper StepMotorUniP = Stepper( stepsPerRevolution, 8, 9, 10, 11);
#else
Stepper StepMotorUniP = Stepper( stepsPerRevolution, 8, 10, 9, 11);
#endif



int iPageCount;
int iIndexCount ; 
int isBootShow= true ;
int iRpmCount=1; 
String StrViewMsg ; 


void pageBoot( U8GLIB_SSD1306_128X32 &U8_SSDInterface );      // Page-1
void pageRPMShow( U8GLIB_SSD1306_128X32 &U8_SSDInterface );   // Page-2
void pageRPMInvert( U8GLIB_SSD1306_128X32 &U8_SSDInterface ); // Page-3
void pageDone( U8GLIB_SSD1306_128X32 &U8_SSDInterface  ) ;    // Page-4

void setupU8GMode( U8GLIB_SSD1306_128X32 &U8_SSDInterface ) ; 


void (*ViewPages[])( U8GLIB_SSD1306_128X32 &U8_SSDInterface ) = { pageBoot, pageRPMShow, pageRPMInvert, pageDone } ;

char *chPageMessage[] = { "Boot", 
                          "Motor run at _X_ rpm.", 
                          "Backward speed",
                          "End Calibration." } ;

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
  U8_SSDInterface.setFont(U8G_FONT_NAME);
  U8_SSDInterface.setColorIndex(1);

}


void setup() 
{  
#ifdef USE_SERIAL_MSG
  Serial.begin(9600);
#endif
  // Setup 4 pin in OUTPUT to trigger each pin by motor - phase 
  pinMode( 8, OUTPUT);
  pinMode( 9, OUTPUT);
  pinMode( 10, OUTPUT);
  pinMode( 11, OUTPUT);
  // Setup 2 pin SDA/SCL for SSD1306 . 
  pinMode( A4, OUTPUT);
  pinMode( A5, OUTPUT);
  
  setupU8GMode( u8g )  ; 
  delay(PAGEDELAY) ; 
  
  iPageCount = 0;
  //Serial.begin(9600);
  
}



void loop() 
{  
#ifdef USE_SERIAL_MSG
  Serial.println(String("Preparing ") + String( MAXPAGE ) + String(" pages to show.") );
#endif

  for( iRpmCount=1 ; iRpmCount <= MAXSPEEDRPM ; iRpmCount++ )
  {
    //u8g.firstPage();
      
    for( iPageCount=0 ; iPageCount <= MAXPAGE-1 ; iPageCount++ )
    {
      u8g.firstPage();
      (*ViewPages[iPageCount])( u8g );
      //incPageCount() ; 
      do 
      {
        delay( PAGEDELAY ) ;
      } while( u8g.nextPage() );      
    } 
  }    
}

void pageBoot( U8GLIB_SSD1306_128X32 &U8_SSDInterface ) 
{
  StrViewMsg = String(chPageMessage[iPageCount]) ;
  if( isBootShow == true )
  { 
    isBootShow=false ;
#ifdef USE_SERIAL_MSG
    Serial.println( StrViewMsg );
#endif
    U8_SSDInterface.drawStr( PIXELXBORDER, PIXELYBORDER, static_cast< const char*>( StrViewMsg.c_str() ) );
    delay(BOOTDELAY) ;    
  }
  

}

void pageRPMShow( U8GLIB_SSD1306_128X32 &U8_SSDInterface )
{
  StrViewMsg = String(chPageMessage[iPageCount]) ;
  StrViewMsg.replace( "_X_", String(iRpmCount) ) ;  
#ifdef USE_SERIAL_MSG
  Serial.println( StrViewMsg );
#endif
  U8_SSDInterface.drawStr( PIXELXBORDER, PIXELYBORDER, static_cast< const char*>( StrViewMsg.c_str() ) );
  StepMotorUniP.setSpeed(iRpmCount);
  StepMotorUniP.step(stepsPerRevolution);
  delay(TIMESPINMOTOR) ; 
}

void pageRPMInvert( U8GLIB_SSD1306_128X32 &U8_SSDInterface ) 
{
  StrViewMsg = String(chPageMessage[iPageCount]) ;
#ifdef USE_SERIAL_MSG
  Serial.println( chPageMessage[iPageCount] );
#endif
  U8_SSDInterface.drawStr( PIXELXBORDER, PIXELYBORDER , static_cast< const char*>( StrViewMsg.c_str() ) ); 
  StepMotorUniP.setSpeed(iRpmCount);
  StepMotorUniP.step(-stepsPerRevolution);
  delay(TIMESPINMOTOR) ; 

}

void pageDone( U8GLIB_SSD1306_128X32 &U8_SSDInterface ) 
{
  StrViewMsg = String(chPageMessage[iPageCount]) ;
#ifdef USE_SERIAL_MSG
  Serial.println( chPageMessage[iPageCount] );
#endif
  U8_SSDInterface.drawStr( PIXELXBORDER, PIXELYBORDER , static_cast< const char*>( StrViewMsg.c_str() ) ); 
  delay(PAGEDELAY) ; 

}
