/*---------------------------------------------------------------------------------------
     
    Micro-controller WEMOS TTG0-X1 LGT8F328 
     
      - Microntroller PIN/NAME for LGT8F328 module

       Pin Name                          Pin Name
       |       Pin Number( on the chip ) |       Pin Number
       |       |                         |       |
       |       |   +------\_/------+     |       |
       |       |   |      USB      |     |       |
       3V3         + (*)       BTN +     5V      
       SWC     22  + LED       RST +    GND
       VIO         +        +--+   +  RESET      27
       D13/SCK 13  +        |SE|   +     D0      
       MOSI    11  +        |RI|   +     D1      
       MISO    12  +        |AL|   +     D2       2
       SWD     23  +        |CH|   +     D3       3
       SCL     19  +        |IP|   +     D4       4
       SDA     18  +        +--+   +     D5       5
       AREF        +     --+-+     +     D6       6
       A0      14  +     --+ |     +     D7       7
       A1      15  +     --+-+     +     D8       8
       A2      16  +    +-----+    +     D9       9
       A3      17  +    |LGT8F|    +    D10      10
       A4      18  +    |328  |    +    D11      11
       A5      19  +    +-----+    +    D12      12 
                   | O           O |
                   +---------------+


  FQBN: lgt8fx:avr:328:variant=modelP-wemos-TTGO-XI
  Using board '328'
  Using core 'lgt8f'
  
  Or 
  
  FQBN: lgt8fx:avr:328
  Using board '328'
  Using core 'lgt8f'
  
  Compilation information:
  
Used library                       Version
WDT                                        
SPI                                1.0     
Adafruit GFX Library               1.12.4  
Adafruit BusIO                     1.17.4  
Wire                               1.0     
Adafruit ST7735 and ST7789 Library 1.11.0  

----------------------------------------------------------------------------------------*/
#ifdef WITH_WDT_LGT8F
#include <WDT.h>
#endif
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>



/*
 * Compiler Define directive configured with -D option from
 * 'arduino-cli compiler' options. 
 * In this section there is on-source defined Directive
 * where it can be changed from arduino-cli compiler. 
 * It use --build-property compiler.c.extra_flags="-DTERMINAL_BAUD=... -DLED_CTRL=... -D...=..."
 * and    --build-property compiler.cpp.extra_flags="-DTERMINAL_BAUD=... -DLED_CTRL=... -D...=..."
 * 
 */ 
#if !defined( TERMINAL_BAUD )
#define TERMINAL_BAUD_SPEED         115200
#else
#define TERMINAL_BAUD_SPEED         TERMINAL_BAUD
#endif

#if !defined( LED_CTRL )
#define LED_CTRL_PIN                12
#else
#define LED_CTRL_PIN                LED_CTRL
#endif

#if !defined( ANALOG_RESOLUTION )
#define ANALOG_RESOLUTION_LVL       10
#else
#define ANALOG_RESOLUTION_LVL       ANALOG_RESOLUTION
#endif

#if !defined( GEN_SLEEP_WAIT )
#define GEN_WAIT_VAL                1000
#else
#define GEN_WAIT_VAL                GEN_SLEEP_WAIT
#endif

#if !defined( I2C_SLAVE_ADDR_MSG )
#define I2C_ADDR_MSG                0x55
#else
#define I2C_ADDR_MSG                I2C_SLAVE_ADDR_MSG
#endif

#if !defined( I2C_CLOCK_FREQ )
#define I2C_CLOCK_FREQUENCY         100000
#else
#define I2C_CLOCK_FREQUENCY         I2C_CLOCK_FREQ
#endif

#if !defined( FLEX_WIRE_PORT_SCL )
#define FLEX_WIRE_SCL               A0
#else
#define FLEX_WIRE_SCL               FLEX_WIRE_PORT_SCL
#endif

#if !defined( FLEX_WIRE_PORT_SDA )
#define FLEX_WIRE_SDA               A1
#else
#define FLEX_WIRE_SDA               FLEX_WIRE_PORT_SDA
#endif

#if !defined( WATCHDOG_ISR_WAIT_TIME )
#define WATCHDOG_ISR_WAIT           WTOH_256MS
#else
#define WATCHDOG_ISR_WAIT           WATCHDOG_ISR_WAIT_TIME
#endif

// Pin Configuration 
/*
 * Encoder only with self push-buton.
 * */
#define ENCDR_A                           2
#define ENCDR_B                           4
#define SW_ENCDR                          3
#define SW_BACK                           5
#define SW_CONF                           6

/*
 * End of Compiler Define directive
 *
 */

// Pin Configuration ST7735S 
/*
 * SCL will refer to SCK from SPI0 interface, Pin13 on wemos TTGO-XI 
 * SDA will refer to MOSI from SPI0 interface, Pin11 on wemos TTGO-XI
 * using Adafruit_ST7735 interface it taking SCL and SDA to SCK/MOSI interface
 * which is non I2C .
 * 
 * */
#define TFT_CS                           10
#define TFT_RST                           8  // Set to -1 if connected to Arduino RESET pin
#define TFT_DC                            9

// I2C Setting. 
//#define DATA_REGISTER_ADDR           0x12

//Rotary Setting
#define ON_TIMEOUT                    5000
#define DEBOUNCING_DELAY                20
#define SW_ENCDR_DEBSMP                  5

//Serial Setting 
#define SERIAL_WAIT_TIME                15

//General Setting

//Color Setting
#define ST77XX_NEON_GREEN             0xc726
#define ST77XX_NEON_GREEN_LIGHT       0xb6c6
#define ST77XX_GREEN_LIME             0xa605
#define ST77XX_GREEN_OLIVE            0x84e4
#define ST77XX_DARK_LIME              0x7424
#define ST77XX_DARK_OLIVE             0x6363
#define ST77XX_GREEN_FOREST           0x4aa2


#define ST77XX_ORANGE_CIRCUIT         0xfba6
#define ST77XX_DARK_ORANGE            0xe325
#define ST77XX_LIGHT_CHERRY           0xe36b
#define ST77XX_ORANGE_CARAMEL         0x9a44
#define ST77XX_BROWN_ORANGE           0x71a3
#define ST77XX_HOT_CHOCOLATE          0x5142


/* FlexWire decl.
 * FlexWire(uint8_t sda = 0, uint8_t scl = 0, bool internal_pullup = false);
 * 
 * Uno or Nano, 
 * SDA : A4 
 * SCL : A5
 * 
 * ESP32 I2C Pin ( connected to FlexWire interface )
 * SDA : 21
 * SCL : 22
 * 
 * Micropython STM32WB55CGU6 I2C
 * 
 * I2C1:
 * SDA: PB7 (Pin 46) or PA10 (Pin 29)
 * SCL: PB6 (Pin 45) or PA9 (Pin 28)
 * 
 * I2C3:
 * SDA: PC1 (Pin 6) or PB4 (Pin 43)
 * SCL: PC0 (Pin 5) or PA7 (Pin 22)
 * 
 * ST32MF101C8T6 I2C
 *  
 * I2C1
 * SDA (Data): Pin 43 (PB7) or Pin 46 (PB9)
 * SCL (Clock): Pin 42 (PB6) or Pin 45 (PB8)
 * 
 * I2C2 
 * SDA (Data): Pin 30 (PB11)
 * SCL (Clock): Pin 29 (PB10)
 * 
 * 
 * */



//Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);


/* FlexWire decl.
 * FlexWire(uint8_t sda = 0, uint8_t scl = 0, bool internal_pullup = false);
 *
 * SoftwareWire decl.
 * SoftwareWire mySoftwareI2C(A0, A1); Pin A0 as SDA, Pin A1 as SCL
 *
 */
 
//SoftWire WireMsgErr(FLEX_WIRE_SDA, FLEX_WIRE_SCL);

//FlexWire        WireMsgErr        = FlexWire(FLEX_WIRE_SDA, FLEX_WIRE_SCL,true);

/*
 * Prototype declaration . 
 * */
void updateScreen( void ) ; 
void rotaryInterrupt( void ) ;
void switchInterrupt( void ) ;
bool buttonPressed(int pin) ; 
void AnimCircle( int xLoc, int yLoc , int *iPtrRad, long *lPtrColorList, int iRepeat, int iDelay) ;

//Interrupt Service routine 
#ifdef WITH_WDT_LGT8F
volatile bool   isrflag              = true;
#endif
volatile bool   lastRotA             = false;
volatile bool   rotarySwitchPressed  = false;
volatile bool   backSwitchPressed    = false;
volatile bool   confirmSwitchPressed = false;
//volatile long   timeout              = 0;
volatile bool   update               = false;
volatile int8_t rotaryMovement       = 0;
volatile int    rotaryCounter        = 50;

bool            IsCounterClkWise     = false ; 


//Global variables
const int       ledPin            = LED_CTRL_PIN; // Led to start when micro-controller is working.
int             timeout           = 0 ;
int             iLoopCount        = 0 ; 
bool            bTimeOutFlag       = true;

long lColorList[] = { ST77XX_NEON_GREEN,   ST77XX_NEON_GREEN_LIGHT,    ST77XX_BLACK,       ST77XX_BLACK, 
                      ST77XX_GREEN_LIME,   ST77XX_BLACK,               ST77XX_BLACK,       ST77XX_GREEN_OLIVE, 
		                  ST77XX_BLACK,        ST77XX_BLACK,               ST77XX_DARK_LIME,   ST77XX_BLACK, 
		                  ST77XX_BLACK,        ST77XX_DARK_OLIVE,          ST77XX_BLACK,       ST77XX_BLACK, 
		                  ST77XX_GREEN_FOREST, ST77XX_BLACK,               ST77XX_BLACK,       ST77XX_NEON_GREEN } ; 

int iRadiusList[] = { 3, 5 } ;


void AnimCircle( int xLoc, int yLoc , int *iPtrRad, long *lPtrColorList, int iRepeat, int iDelay )
{
  for( int iC = 0 ; iC <= iRepeat ; iC ++ )
  {
    for( int iCRad = 0 ; iCRad <= ( int )(sizeof( iPtrRad ))-1 ; iCRad++ )
    {
      for( int iCol = 0 ; iCol <= ( int )(sizeof( lPtrColorList ))-1 ; iCol++ )
      {
	      tft.fillCircle(xLoc, yLoc, ( int16_t )( *(iPtrRad+iCRad) ), ( int16_t )( *(lPtrColorList+iCol) ) );
	      //delay(GEN_WAIT_VAL/4);
	      delay(iDelay);
	      tft.fillCircle(xLoc, yLoc, ( int16_t )( *(iPtrRad+iCRad) ), ST77XX_BLACK );
      }
    }
  }
}

void rotaryInterrupt( void )
{
  /*
   Rotary rotation detection.
  */
  if (!digitalRead(ENCDR_A) && lastRotA)
  {
    rotaryMovement = (digitalRead(ENCDR_B)) ? -1 : 1;
  }
  lastRotA = digitalRead(ENCDR_A);
  
  /*
   Rotary switch detection.
  */
  //switchInterrupt();
  //rotarySwitchPressed = (digitalRead(SW_ENCDR) == LOW);
  
}

//---------------------------------------------------------------------
// Rotary encoder was pressed
//---------------------------------------------------------------------

void switchInterrupt( void )
{
#ifdef DEBOUNCING_SW
  int xC = 0 ; 
  int iCheckLOW = 0 ;
  int iCheckHIGH = 0 ;  
  //bool isCheck = false ; 
  /*
   De-Bouncing Switch from encoder for possible problem 
   to read the statement. This method create n digitalRead(SW_ENCDR)
   and for every count it add 1 to HIGH or LOW read and compare at 
   the end which one as the greatest number of count. This hapenning
   in some micro-controller fast enough to offer a no answer of delayed
   retransmitted value that might affect the initial touch the Rotary 
   button as button not a roller. Like STM32F103C8T6 does not require
   to debounce the digitalRead(SW_ENCDR) it read and clearly give an
   HIGH or LOW signal . Here with LGT8F328 it seem to require it. 
  */
  bool bPressSample[SW_ENCDR_DEBSMP] ; 
#ifdef WITH_WDT_LGT8F
  if( !isrflag )
  {
#endif
    while ( xC <= SW_ENCDR_DEBSMP)
    {
      bPressSample[xC] = (digitalRead(SW_ENCDR) == LOW) ;
      for ( int iC=0 ; iC <= DEBOUNCING_DELAY ; iC++)
      { ; }
      if( xC > 0 )
      {
	if( bPressSample[xC] == true )
	{
	  iCheckLOW++ ; 
	}
	else
	{
	  iCheckHIGH++;
	}
      }
      xC++ ; 
    } ;
    if( iCheckLOW >= iCheckHIGH )
    {
      rotarySwitchPressed = true;
    }
    if( iCheckHIGH >= iCheckLOW )
    {
      rotarySwitchPressed = false;
    }
#ifdef WITH_WDT_LGT8F  
  }
#endif
#else
  rotarySwitchPressed = (bool)(digitalRead( SW_ENCDR ) == LOW) ;
  //rotarySwitchPressed = buttonPressed( SW_ENCDR );
#endif
}

bool buttonPressed(int pin)
{
  bool pressed = false;
  if (digitalRead(pin) == LOW)
  {
    //delay(DEBOUNCING_DELAY);
    for( int iC = 0 ; iC <= 350; iC++ ) ;
    {
    }
    if (digitalRead(pin) == LOW)
    {
      while (digitalRead(pin) == LOW)
      {
        //yield();
      }
      pressed = true;
    }
  }
  return pressed;
}

void UpdatePressAction( void )
{
  if (rotarySwitchPressed)
  {
    //Serial.println("Rotary Switch Pressed");
    update = true;
  }
  
  if (buttonPressed( SW_BACK ))
  {
    //Serial.println("Rotary Switch Pressed");
    backSwitchPressed=true;
    update = true;
  }

  if (buttonPressed( SW_CONF ))
  {
    //Serial.println("Rotary Switch Pressed");
    confirmSwitchPressed=true;
    update = true;
  }
  
  if (rotaryMovement != 0)
  {
    //Serial.print("Rotary encoder moved ");
    //Serial.println((rotaryMovement > 0) ? "left" : "right");
    rotaryCounter += (rotaryMovement > 0) ? -1 : 1 ;
    IsCounterClkWise=(rotaryMovement > 0) ? false : true ; 
    
    rotaryCounter = min(max((int)rotaryCounter, 0), 99);
    rotaryMovement = 0;
    update = true;
  }
  
  if (update == true)
  {
    //Serial.println("Update function entry");
    update = false;
    //timeout = millis() + ON_TIMEOUT;
    //Serial.print("Timeout entry is ");
    //Serial.println(timeout);
    updateScreen() ; 
    rotarySwitchPressed  = false;
    backSwitchPressed    = false;
    confirmSwitchPressed = false;
    //Serial.println("Update function exit");
  }

}

void updateScreen( void )
{
  //ST77XX_RED
  //tft.drawRect(0, 0, 121, 17, ST77XX_BLUE);
  tft.fillRect(1, 25, 120, 10, ST77XX_ORANGE_CIRCUIT);
  //tft.fillRect(1, 35, 119, 10, ST77XX_LIGHT_CHERRY);
  tft.fillRect(1, 35, 120, 10, ST77XX_DARK_ORANGE);
  /*
   * Background color for ENCDR:
   *                      BACK:
   *                      CONF:
   * */
  tft.fillRect(1, 45, 70, 10, ST77XX_ORANGE_CARAMEL);
  tft.fillRect(1, 55, 70, 10, ST77XX_BROWN_ORANGE);
  tft.fillRect(1, 65, 70, 10, ST77XX_HOT_CHOCOLATE) ; 
  
  
  // Print secondary text over the filled shape
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.setCursor(20, 27);
  tft.println("[Rotary Dir]");
  tft.setCursor(20, 37);
  tft.println("R.Count:" + String(rotaryCounter));
  
  tft.setCursor(20, 47);
  tft.println("<- ENCDR");
  tft.setCursor(20, 57);
  tft.println("<- BACK");
  tft.setCursor(20, 67);
  tft.println("<- CONF");
  
  
  //ST77XX_YELLOW
  //iColorList = ( int* )( malloc( 2 * sizeof(int) ) ) ;
  //iRadiusList = ( int* )( malloc( 2 * sizeof(int) ) ) ;
  if( IsCounterClkWise )
  {
    //tft.setCursor(5, 27);
    //tft.println("  [Rotary Dir]->");
    //void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2,int16_t y2, uint16_t color);
    tft.fillTriangle(120, 34, 105, 26, 105 ,42, ST77XX_NEON_GREEN_LIGHT);
  } 
  else
  {
    //tft.setCursor(5, 27);
    //tft.println("<-[Rotary Dir]  ");
    tft.fillTriangle(105, 34, 120, 26, 120 ,42, ST77XX_NEON_GREEN_LIGHT);
  }
  if ( rotarySwitchPressed )
  {
    AnimCircle( 7, 49 , iRadiusList, lColorList, 2, GEN_WAIT_VAL/6 ) ;
    tft.fillRect(1, 35, 15, 10, ST77XX_DARK_ORANGE);
    tft.fillRect(1, 45, 15, 10, ST77XX_ORANGE_CARAMEL);;
  }

  if ( backSwitchPressed )
  {
    AnimCircle( 7, 59 , iRadiusList, lColorList, 2, GEN_WAIT_VAL/6) ;
    tft.fillRect(1, 45, 15, 10, ST77XX_ORANGE_CARAMEL);
    tft.fillRect(1, 55, 15, 10, ST77XX_BROWN_ORANGE);
  }

  if ( confirmSwitchPressed )
  {
    AnimCircle( 7, 69 , iRadiusList, lColorList, 2, GEN_WAIT_VAL/6) ;
    tft.fillRect(1, 55, 15, 10, ST77XX_BROWN_ORANGE);
    tft.fillRect(1, 65, 15, 10, ST77XX_HOT_CHOCOLATE) ;
  }

}


//---------------------------------------------------------------------
// Interrup Service routine for ARDUINO_AVR_LARDU_328E.
//---------------------------------------------------------------------

#ifdef WITH_WDT_LGT8F
ISR (WDT_vect)
{
  isrflag = true;
  
}
#endif

//---------------------------------------------------------------------
// Setup Hardware
//---------------------------------------------------------------------

void setup() 
{
  delay(GEN_WAIT_VAL*3);
  
#ifdef LED_CTRL_PIN
  pinMode(ledPin,INPUT);
  digitalWrite(ledPin, LOW);
#endif
  
  pinMode( ENCDR_A  ,INPUT );
  pinMode( ENCDR_B  ,INPUT );
  pinMode( SW_ENCDR ,INPUT_PULLUP );
  pinMode( SW_BACK  ,INPUT_PULLUP );
  pinMode( SW_CONF  ,INPUT_PULLUP );
  
  digitalWrite(SW_ENCDR, LOW) ;
  
#ifdef DEBUG_USB_SERIAL
  Serial.begin( TERMINAL_BAUD_SPEED );
  while (!Serial) 
  {
    delay(SERIAL_WAIT_TIME) ; 
  }
  Serial.flush();
#endif
  
  delay(GEN_WAIT_VAL);
#ifdef DEBUG_USB_SERIAL
  Serial.print("\r\n") ; 
  Serial.println( "BOOTING" );
  Serial.println("LGT8F328 ST7735S 80x160 SPI LCD test.\r\nAverage time lease " + 
		 String(ON_TIMEOUT) + String( " ms." ) );
#endif 

#ifdef LED_CTRL_PIN
  digitalWrite(ledPin, HIGH);
#endif
  
  attachInterrupt(digitalPinToInterrupt(SW_ENCDR), switchInterrupt, FALLING );
  attachInterrupt(digitalPinToInterrupt(ENCDR_A), rotaryInterrupt, CHANGE);
  
  
  // Initialize OLED
  delay(GEN_WAIT_VAL);

  // Screen Initialisation. 
  //tft.initR(INITR_BLACKTAB); 
  //tft.initR(INITR_REDTAB);
  tft.initR(INITR_GREENTAB);
  tft.invertDisplay(true);
  tft.setRotation(3); 
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextWrap(true);
  //tft.setTextColor(ST77XX_GREEN);
  //tft.setTextSize(1);
  //tft.setCursor(10, 0);
  //tft.println("ST7735S OK");
  
  delay(GEN_WAIT_VAL);  
  //WireMsgErr.begin(); 
  
  timeout = millis() + ON_TIMEOUT;
#ifdef WITH_WDT_LGT8F
  Lgtwdt.begin(WTOH_32MHZ);
  //wdt_ienable(WTO_2S);
  //wdt_ienable(WTOH_512MS); 
  wdt_ienable(WATCHDOG_ISR_WAIT);
#endif

  updateScreen();
}

//---------------------------------------------------------------------
// Main loop
//---------------------------------------------------------------------

void loop() 
{
  update = false;
  iLoopCount=0;
  do 
  {
    UpdatePressAction() ;

#ifdef WITH_WDT_LGT8F    
    if( isrflag && (int)(millis()) <= timeout)
#else
    if( (int)(millis()) <= timeout)
#endif
    {
      //updateScreen();
#ifdef DEBUG_USB_SERIAL      
      Serial.println("SSD1306 Screen Updated." );
#endif      
#ifdef WITH_WDT_LGT8F
      isrflag=false;
      //wdt_ienable(WTO_4S);
      //wdt_ienable(WTOH_512MS);
      wdt_ienable( WATCHDOG_ISR_WAIT );
#endif
    }
    

    if (timeout > 0 && (int)(millis()) >= timeout)
    {
      timeout = millis() + ON_TIMEOUT;
      bTimeOutFlag=true;
      update=false;
      //iReturnWError = WireMsgErr.endTransmission();
      //Serial.println("FLexWire/SoftWire endTransmission for expired lease return :" + String( iReturnWError ) ); 
    }
    
    if( bTimeOutFlag  )
    {
#ifdef DEBUG_USB_SERIAL      
      Serial.println("Cycle " + String(iLoopCount) + " end, Warning Timeout !!!" ); 
#endif      
      bTimeOutFlag=false;
    }
    iLoopCount++ ;
    delay(GEN_WAIT_VAL/100);

  } while( iLoopCount < 1000 );
  
}
