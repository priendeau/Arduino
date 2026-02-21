#include <Arduino.h>
#include "SoftwareReset.hpp"

/*
  Adapted from :
  --------------------------------------
  74HC165 Shift Register Demonstration 1
  
  DroneBot Workshop 2020
  https://dronebotworkshop.com
  --------------------------------------
  filename: DrnBtShiftRegPISO.ino

  Description:
  Read from 8 switches and display values on serial monitor
 
*/

#if !defined( SHIFT_REG_COUNT_BIT )
#define SHIFT_REG_NB_STATE          8
#else
#define SHIFT_REG_NB_STATE          SHIFT_REG_COUNT_BIT
#endif


#define DEFAULT_SLEEP               10000
#define IO_SLEEP                    15
#define TRIAL_COUNT                 100

#if !defined( TERMINAL_BAUD )
#define TERMINAL_BAUD_SPEED         9600
#else
#define TERMINAL_BAUD_SPEED         TERMINAL_BAUD
#endif

#define DEFAULT_NOT_VALUE           -1

#define MAJOR_VERSION               1
#define MINOR_VERSION               922

#define HOSTNAME                    "DrnBtShiftRegPISO"
// Define Connections to 74HC165

// CE pin 15
int clockEnablePin    = 4;
// Q7 pin 7
int dataIn            = 5;
// CP pin 2
int clockIn           = 6;
// PL pin 1
int load              = 7;

/* Define program specific variable
 * */
int iReadNb           = 0 ;
const int ledPin      = 12; // Led to start when micro-controller is working.
byte incoming ; 
//char *PrintBuffer ; 

 
int *iStateCounter    = NULL ; 
int iMaxStateCount ;
int iCountingState    = 0 ; 
bool bCurrentRegister = false; 

String decimalToBinary(int value)
{
  String result = "";
  int iBitPadLeft = SHIFT_REG_NB_STATE ; 
  //int iNumFillBit = 0 ; 
  if(value == 0)
  {
    result = "00000000";
  }
  else
  {
    while(value > 0)
    {
      if((value % 2) == 0)
      {
        result = "0" + result;
      }
      else
      {
        result = "1" + result;
      }
      value /= 2;
      //iNumFillBit++ ; 
      iBitPadLeft-- ; 
    }
    for( int iC = 0 ; iC < iBitPadLeft ; iC++)
    {
		 result = "0" + result; 
		}
  }
  return result;
}


void InitShifStateCounter()
{
  iMaxStateCount = pow(SHIFT_REG_NB_STATE,2) ;
  iStateCounter = ( int* )malloc( static_cast< size_t>( iMaxStateCount ) * sizeof(int) ) ;	
	for( int iCount=0 ; iCount < iMaxStateCount; iCount++ )
	{
		*(iStateCounter + iCount) = DEFAULT_NOT_VALUE ; 
	}
	iCountingState=0 ;
}


bool ShiftStateCounter( int value )
{
	bool isStateFound = false ;
	bool isUpdated    = false ;
  //int iValuePtr ;
	if( iCountingState < iMaxStateCount )
	{
		for( int iCount=0 ; iCount < iMaxStateCount; iCount++ )
		{
      //iValuePtr = *(iStateCounter + iCount) ;
			if( *(iStateCounter + iCount) == value )
			{
				isStateFound=true;
			}
		}
		if( !isStateFound )
		{
			for( int iCount=0 ; iCount < iMaxStateCount; iCount++ )
			{
        //iValuePtr = *(iStateCounter + iCount) ; 
				if( !isUpdated )
				{
					if( *(iStateCounter + iCount) == DEFAULT_NOT_VALUE )
					{
            *(iStateCounter + iCount)=value;
						isUpdated=true;
            iCountingState++ ; 
					}
				}
			}
		}
	}
  return isStateFound ; 
} 
 

void DisplayStateCount( void )
{
  Serial.println( "Reporting all register-state discovered ." );
  int iValuePrint;
	for( int iCount=0 ; iCount < iCountingState; iCount++ )
  {
    iValuePrint=*(iStateCounter + iCount);
    if( iValuePrint != DEFAULT_NOT_VALUE )
    {
      /* Printing on terminal the rank.
       * */
      if( (iCount+1) > 0 && (iCount+1) <= 9 )
      {
        Serial.print( String( iCount+1 ) + "-  ") ;
      }
      if( (iCount+1) > 9 && (iCount+1) <= 100 )
      {
        Serial.print( String( iCount+1 ) + "- ") ;
      }
      delayMicroseconds(IO_SLEEP/5) ;
  
      /* Printing on terminal the value and it's binary conversion.
       * */
      if( iValuePrint >= 0 && iValuePrint <= 9 )
      {
        Serial.print( "Value: " + String( iValuePrint ) + "   Pin State: " );
      }
      if( iValuePrint >= 10 && iValuePrint <= 100 )
      {
        Serial.print( "Value: " + String( iValuePrint ) + "  Pin State: " );
      }
      if( iValuePrint > 100 )
      {
        Serial.print( "Value: " + String( iValuePrint ) + " Pin State: " );
      }
      delayMicroseconds(IO_SLEEP/5) ;
      
      Serial.println( String( decimalToBinary( static_cast< int>( iValuePrint ) ) ) ) ; 
      delayMicroseconds(IO_SLEEP/5) ;
    }
  }
}


void setup()
{
  iReadNb=0;
  /* It require to allocating the size of the number possible state
   * a shift-register can count by chip you install on a micro-controller.
   * - For one shift-register in PISO mode it count 8 states.
   * - For two shift-register in PISO mode it count 16 states.
   * Be sure you have SHIFT_REG_NB_STATE set to 8 or call a property
   * From arduino-cli for SHIFT_REG_COUNT_BIT set to the desired number 
   * of state possible.
   * in this case calling arduino-cli compiler will require 
   * 
   * StrDefineLine="-DTERMINAL_BAUD=9600 -DSHIFT_REG_COUNT_BIT=8"
   * arduino-cli compiler --build-property compiler.c.extra_flags="${StrDefineLine}" compiler.cpp.extra_flags="${StrDefineLine}"
   * 
   * */
  InitShifStateCounter() ;
  
  // Setup Serial Monitor
  Serial.begin(TERMINAL_BAUD_SPEED);
  while ( !Serial )
  {
    delay(IO_SLEEP);
  }

  delay(DEFAULT_SLEEP); 
  // Setup 74HC165 connections
  pinMode(load, OUTPUT);
  pinMode(clockEnablePin, OUTPUT);
  pinMode(clockIn, OUTPUT);
  pinMode(dataIn, INPUT);
  
  /* Belong to Board light-pin to light up after a boot.
   * */
  pinMode(ledPin,INPUT);
  digitalWrite(ledPin, HIGH);  
  
  delay(DEFAULT_SLEEP);
  Serial.print("\r\n") ; 
  Serial.println( "BOOTING" );
  Serial.println("LGT8F328 wih Shift Register 74HC165");
  Serial.println( String( HOSTNAME ) + " Version " + String(MAJOR_VERSION) +"."+ String(MINOR_VERSION)+ "\r\n") ;
  Serial.println( "System is allocating " + String( iMaxStateCount ) + " slots to store PISO registry found.") ;
  Serial.println("\nWarning !!!\r\nAfter micro-controller work's done it reboot and start again.\r\n") ;
}
 
void loop()
{
  iReadNb++ ; 
  // Write pulse to load pin
  digitalWrite(load, LOW);
  delayMicroseconds(IO_SLEEP/3);
  digitalWrite(load, HIGH);
  delayMicroseconds(IO_SLEEP/3);
 
  // Get data from 74HC165
  digitalWrite(clockIn, HIGH);
  digitalWrite(clockEnablePin, LOW);
  incoming = 0 ; 
  delayMicroseconds(IO_SLEEP/3);
  incoming = shiftIn(dataIn, clockIn, LSBFIRST);
  delayMicroseconds(IO_SLEEP/3);
  digitalWrite(clockEnablePin, HIGH);
  
  // Print to serial monitor
  Serial.println( "--------------------");
  Serial.println( "Trial number: " + String( iReadNb ) );  
  Serial.println( "Pin States: " + decimalToBinary( static_cast< int>( incoming ) ) );
  Serial.println( "Register Value: " + String( static_cast< int>( incoming ) ) );
  
  bCurrentRegister = ShiftStateCounter( static_cast< int>( incoming ) ) ;
  if ( bCurrentRegister == true )
  {
    Serial.println( "New state discover." );
    bCurrentRegister=false;
  } 
  
  delay(DEFAULT_SLEEP/5) ; 
  
  if( iReadNb == TRIAL_COUNT )
  {
    Serial.println(); 
    DisplayStateCount() ; 

    iReadNb=0;
    Serial.println("\nPlease stand-by, micro-controller is rebooting after "+String( TRIAL_COUNT )+" trials.");
    /* Turning the board-light off. 
     * */
    digitalWrite(ledPin, LOW);
    delay(DEFAULT_SLEEP/5);
    softwareReset::standard() ;
  }
}
