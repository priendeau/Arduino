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


#if !defined( TFT_DC_PIN )
#define TFT_DC                      9
#else
#define TFT_DC		                  TFT_DC_PIN                
#endif

#if !defined( TFT_CS_PIN )
#define TFT_CS                      10
#else
#define TFT_CS		                  TFT_CS_PIN                
#endif

#if !defined( TFT_RST_PIN )
#define TFT_RST                     8
#else
#define TFT_RST		                  TFT_RST_PIN                
#endif

#if defined( WITH_TFT_DIMMING )

#if !defined( TFT_PWM_DIM_PIN )
#define TFT_PWM_DIM                 6
#else
#define TFT_PWM_DIM		              TFT_PWM_DIM_PIN                
#endif

#if !defined( DIMMING_PWM_LVL )
#define PWM_DIM_LEVEL               32
#else
#define PWM_DIM_LEVEL		            DIMMING_PWM_LVL                
#endif

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

#if !defined( MAIN_VOLTAGE )
#define BOARD_VOLTAGE              3.2750
#else
#define BOARD_VOLTAGE              MAIN_VOLTAGE
#endif

#if !defined( SAMPLING_READ )
#define SAMPLING_VALUE              5000
#else
#define SAMPLING_VALUE              SAMPLING_READ
#endif

#if !defined( SLEEP_READING )
#define SLEEP_INTERVAL              200
#else
#define SLEEP_INTERVAL              SLEEP_READING
#endif

#if !defined( VOLTAGE_PIN_DIVIDER )
#define VOLTAGE_DIVIDER             A0
#else
#define VOLTAGE_DIVIDER             VOLTAGE_PIN_DIVIDER
#endif

#if !defined( LM35_PIN_1_NAME )
#define LM35_PIN_1                  A1
#else
#define LM35_PIN_1                  LM35_PIN_1_NAME
#endif

#if !defined( LM35_PIN_2_NAME )
#define LM35_PIN_2                  A2
#else
#define LM35_PIN_2                  LM35_PIN_2_NAME
#endif

#if !defined( LM35_PIN_PWR )
#define LM35_POWER                  8
#else
#define LM35_POWER                  LM35_PIN_PWR
#endif

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
 * In case define TFT_DC/TFT_CS/TFT_RES just look inside the define include/section it was moved
 * into callable define for arduino-cli .
 * 
 * */



//Serial Setting 
#define SERIAL_WAIT_TIME                15

//General Setting

//Color Setting

//Color Yellow like definition.
#define ST77XX_YELLOW_DARK_MAZEN      0xde25

//Color Purple like definition.
#define ST77XX_PURPLE_SMARTIES        0x5130
#define ST77XX_DARK_PURPLE            0x48ac

//Color Green like
#define ST77XX_NEON_GREEN             0xc726
#define ST77XX_NEON_GREEN_LIGHT       0xb6c6
#define ST77XX_GREEN_LIME             0xa605
#define ST77XX_GREEN_OLIVE            0x84e4
#define ST77XX_DARK_LIME              0x7424
#define ST77XX_DARK_OLIVE             0x6363
#define ST77XX_GREEN_FOREST           0x4aa2

//Color between orange and brown definition.. 
#define ST77XX_ORANGE_CIRCUIT         0xfba6
#define ST77XX_DARK_ORANGE            0xe325
#define ST77XX_LIGHT_CHERRY           0xe36b
#define ST77XX_ORANGE_CARAMEL         0x9a44
#define ST77XX_BROWN_ORANGE           0x71a3
#define ST77XX_HOT_CHOCOLATE          0x5142

/*
 * Common define for help resolving pointer-relation
 * and oriented named value. 3 value concurrent value
 * are Sensor1, Sensor2, and the voltage-divider sensing.
 * In all the case a float value in pointer with SENS_LM35_1
 * will represent all the value collected to end into result
 * for SENS_LM35_1. A long value is also used with pointer 
 * and helping identifying is use will also address to value 
 * of SENS_LM35_1. As Vref value is an exception and only 
 * interact with nominal voltage SENS_VREF is for merging 
 * vRef value into adcSum .
 * */
#define SENS_LM35_1                   0
#define SENS_LM35_2                   1
#define SENS_VREF                     2
#define SENS_LM35_1_FARENHEIGHT       2
#define VOLTAGE_REAL                  2

typedef enum enum_GfxProc
{
	DRAW_NONE     = 0 , 
  DRAW_FILLRECT	= 1 ,
  DRAW_RECT     = 2 ,
  DRAW_LABEL    = 3 ,
  DRAW_VAR      = 4 , 
} eGfxProc ;

typedef struct st_GfxSampling 
{ 
	eGfxProc eName ; 
	int x ;
	int y ; 
	int width ;
	int height ;
  int color ;
  char *cLabel ;   
} stGfxSampling_t ;

stGfxSampling_t DrawRegion[] = {   
				{ DRAW_RECT    ,  3,  3,  68, 41, ST77XX_BLUE              , NULL     } ,
				{ DRAW_FILLRECT, 95, 26,  60, 10, ST77XX_BLACK             , NULL     } ,
				{ DRAW_FILLRECT, 95, 36,  60, 10, ST77XX_BLACK             , NULL     } , 
				{ DRAW_FILLRECT, 40, 45,  40, 10, ST77XX_BLACK             , NULL     } , 
				{ DRAW_FILLRECT, 40, 55,  60, 10, ST77XX_BLACK             , NULL     } , 
				{ DRAW_FILLRECT, 40, 65, 110, 10, ST77XX_BLACK             , NULL     } ,
				{ DRAW_FILLRECT, 40, 75,  70, 10, ST77XX_BLACK             , NULL     } , 
				{ DRAW_FILLRECT, 40, 85,  60, 10, ST77XX_BLACK             , NULL     } ,
				{ DRAW_LABEL   , 20, 26,   0,  0, ST77XX_WHITE             , "LM35D"  } ,
				{ DRAW_LABEL   , 20, 34,   0,  0, ST77XX_WHITE             , "TEMP"   } , 
				{ DRAW_LABEL   , 73, 28,   0,  0, ST77XX_YELLOW_DARK_MAZEN , "A2v:"   } ,
				{ DRAW_VAR     , 95, 28,   0,  0, ST77XX_YELLOW_DARK_MAZEN , NULL     } ,
				{ DRAW_LABEL   , 73, 38,   0,  0, ST77XX_YELLOW_DARK_MAZEN , "A2t:"   } ,
				{ DRAW_VAR     , 95, 38,   0,  0, ST77XX_YELLOW_DARK_MAZEN , NULL     } ,  
				{ DRAW_LABEL   ,  5, 46,   0,  0, ST77XX_YELLOW_DARK_MAZEN , "ADC  :" } ,  
				{ DRAW_VAR     , 42, 46,   0,  0, ST77XX_YELLOW_DARK_MAZEN , NULL     } ,
				{ DRAW_LABEL   ,  5, 56,   0,  0, ST77XX_YELLOW_DARK_MAZEN , "Volt :" } ,
				{ DRAW_VAR     , 42, 56,   0,  0, ST77XX_YELLOW_DARK_MAZEN , NULL     } , 
				{ DRAW_LABEL   ,  5, 66,   0,  0, ST77XX_YELLOW_DARK_MAZEN , "Temp :" } ,
				{ DRAW_VAR     , 42, 66,   0,  0, ST77XX_YELLOW_DARK_MAZEN , NULL     } ,  
				{ DRAW_LABEL   ,  5, 76,   0,  0, ST77XX_YELLOW_DARK_MAZEN , "D.Res:" } , 
				{ DRAW_VAR     , 42, 76,   0,  0, ST77XX_YELLOW_DARK_MAZEN , NULL     } ,
				{ DRAW_LABEL   ,  5, 86,   0,  0, ST77XX_YELLOW_DARK_MAZEN, "VRef :"  } , 
				{ DRAW_VAR     , 42, 86,   0,  0, ST77XX_YELLOW_DARK_MAZEN , NULL     }  
	     } ;

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

void DrawSection( eGfxProc eDrawCond = DRAW_NONE, int iPos=0, char* cPrintVar = NULL) ; 

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST) ;

const int vRef_pin 	    = VOLTAGE_DIVIDER ; 
const int vPwr_pin      = LM35_POWER ;
const int sensorPin 	  = LM35_PIN_1 ; 
const int sensorPin1    = LM35_PIN_2 ;

#if defined( WITH_TFT_DIMMING )
const int PwmDimPin     = TFT_PWM_DIM ; 
int       PwmLevelDim   = PWM_DIM_LEVEL ; 
#endif

long  *adcSum ;
float *voltage_mV ; 
float *fTemp ; 

int iAnalogRes                = ANALOG_RESOLUTION ; 
const int totalSamples        = SAMPLING_VALUE ;
const int iSleepCollect       = SLEEP_INTERVAL ;
const float VCC_VOLTAGE       = BOARD_VOLTAGE ;
//const float VCC_VOLTAGE   = 3.3 ; 


const float ADC_RESOLUTION = ( pow(2,iAnalogRes)-1 ) ;

void DrawSection( eGfxProc eDrawCond = DRAW_NONE, int iPos=0, char* cPrintVar = NULL )
{
	int iC=0 ;
	if( eDrawCond == DRAW_NONE )
	{
		for ( iC=0 ; iC <= (sizeof(DrawRegion)/sizeof( stGfxSampling_t ))-1 ; iC++ )
		{
			switch( DrawRegion[iC].eName )
			{
				case DRAW_FILLRECT :
				tft.fillRect( DrawRegion[iC].x, 
											DrawRegion[iC].y, 
											DrawRegion[iC].width, 
											DrawRegion[iC].height, 
											DrawRegion[iC].color ) ;
				break;
				case DRAW_RECT :
				tft.drawRect( DrawRegion[iC].x, 
											DrawRegion[iC].y, 
											DrawRegion[iC].width, 
											DrawRegion[iC].height, 
											DrawRegion[iC].color );
				break ; 
				case DRAW_LABEL :
				tft.setCursor( DrawRegion[iC].x, 
											 DrawRegion[iC].y );
				tft.setTextColor( DrawRegion[iC].color );
				tft.println( DrawRegion[iC].cLabel );
				break;
			}
		}
	}
	if( eDrawCond == DRAW_VAR )
	{
		tft.setCursor( DrawRegion[iPos].x, 
									 DrawRegion[iPos].y );
		tft.setTextColor( DrawRegion[iPos].color );
		tft.println( String( cPrintVar ) );
	}
	if( eDrawCond == DRAW_FILLRECT )
	{
		for ( iC=0 ; iC <= (sizeof(DrawRegion)/sizeof( stGfxSampling_t ))-1 ; iC++ )
		{
			if( DrawRegion[iC].eName == DRAW_FILLRECT )
			{
				tft.fillRect( DrawRegion[iC].x, 
											DrawRegion[iC].y, 
											DrawRegion[iC].width, 
											DrawRegion[iC].height, 
											DrawRegion[iC].color ) ;
			}
		}
	}
}

//---------------------------------------------------------------------
// Interrup Service routine for ARDUINO_AVR_LARDU_328E.
//---------------------------------------------------------------------


//---------------------------------------------------------------------
// Setup Hardware
//---------------------------------------------------------------------

void setup() 
{
  delay(GEN_WAIT_VAL*3);

#ifdef ARDUINO_AVR_LARDU_328E
  analogReference(DEFAULT);
  //analogReference( INTERNAL1V024 );
  //analogReference( INTERNAL2V048 );
  //analogReference( INTERNAL4V096 );
#ifdef ANALOG_RESOLUTION
  analogReadResolution(iAnalogRes);
#endif
#endif	 
	/* Managing sensor read of 2 INPUT for 2 LM35. 
	 * */
  pinMode(sensorPin, INPUT);
  pinMode(sensorPin1, INPUT) ; 
  
  /* Managing the power-voltage level from a divider 
   * */
  pinMode(vRef_pin, INPUT) ; 
  
  
  /* Managing power for 2 LM35D
   * */
  pinMode(vPwr_pin, OUTPUT) ;
  /* Starting in OFF-mode 
   * */ 
  digitalWrite(vPwr_pin, LOW);

  
  // Initialize OLED
  delay(GEN_WAIT_VAL);

  // Screen Initialisation. 
  tft.initR(INITR_GREENTAB);
  //tft.initR(INITR_MINI160x80);
  tft.invertDisplay(true);
  tft.setRotation(3); 
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextWrap(true);

#if defined( WITH_TFT_DIMMING )
	/* Managing Pwm Pin Dimming to make the ST7735 being 
	 * able to get shot from camera without having blur 
	 * effect
	 * */
	pinMode(PwmDimPin, OUTPUT) ;
	analogWrite(PwmDimPin, PwmLevelDim);
#endif 

  delay(GEN_WAIT_VAL);  
  
  tft.setTextSize(1);
  /* 
   * Draw-All because the filter is at DRAW_NONE by default and doesn't 
   * filter. 
   * */
  DrawSection( ) ; 
  /* Declaring the pointer-float here, 2 unit are required.  
   * Both getting value from ADC turn into Voltage . 1 Unit
   * will serve stocking the voltage divider relation . In total
   * 3 unit. 
   * */
  voltage_mV = (float*)( malloc( sizeof(float) * 3 ) ) ;
  
  /* Declaring the pointer-long here, 2 unit are required.
   * Both are adder for turning analogRead value into a long-number
   * divider by the number of sampling for getting average value,
   * Vref being added here 
   * */
  adcSum = (long*)( malloc( sizeof(long) * 3 ) ) ;
 
	/* Declaring the pointer-float for 3 entity that report 
	 * 2 different temperature in Celsius. Combined togheter to 
	 * remove the variable Supply_temp it's now another fTemp.
	 * The third-one is a conversion from Celsius to Farenheight
	 * */
	fTemp = (float*)( malloc( sizeof(float) * 3 ) ) ;
	
}



//---------------------------------------------------------------------
// Main loop
//---------------------------------------------------------------------

void loop() 
{
  *(adcSum+SENS_LM35_1) = 0;
  *(adcSum+SENS_LM35_2) = 0;
  *(adcSum+SENS_VREF)   = 0;
  
  /*
   * Getting the voltage reference from A0.
   * 
   * if analogReference(DEFAULT);
   * vRef is reporting value between 0 - (2^iAnalogRes)-1, which is not 
   * voltage. It's a scale of 3.3 volts over 2^iAnalogRes step from 0
   * to (2^iAnalogRes)-1.
   * if analogReference( INTERNAL2V048 );
   * It's a scale of 2.048 volts over 2^iAnalogRes step from 0
   * to (2^iAnalogRes)-1.
   * 
   * An applied 2 time 10k Ohms resistor where the vRef_pin is 
   * between them should drive the voltage to be split in half
   * and do require to tell the vRef should be multiplied by 2
   * to give the nominal voltage. 
   * Notice : I found excellent 9.99 kOhms resistor at 1/4 watts
   * severals resistor were gauge, hence to get difficult perfect 10k
   * it's average start from 9.85 kOhms to 10.01 kOhms. But it's 
   * strongly required to find exact match Ohmistic value to 
   * allow the internal divider to be a value of 2, to get back 
   * 3.27 volts from ams1117-3.3, does not deliver 3.3 but close
   * as 3.27 volts. It's possible the whole micro-controller is
   * under-powered and lead to false result. 
   * */
  /* Start the power for 2 LM35 */
  digitalWrite(vPwr_pin, HIGH);
  delay(GEN_WAIT_VAL/5) ; 
  
  for (int i = 0; i < totalSamples; i++) 
  {
    /* Part-1 collecting voltage from the divider.*/
    *(adcSum+SENS_VREF) += analogRead(vRef_pin);
    delay( iSleepCollect/2 ); // Small delay between samples
    
    /* Patrt-2 Collecting LM35-1 and LM35-2 value */
    *(adcSum+SENS_LM35_1) += analogRead(sensorPin);
		delay( iSleepCollect/3 );
		
		*(adcSum+SENS_LM35_2) += analogRead(sensorPin1);
    delay( iSleepCollect/3 ); // Small delay between samples
  }
  
  /* *(voltage_mV+VOLTAGE_REAL) vcc-real value to propose instead of a 
   * fixed value it depend on the respond of the voltage * 2 because 
   * it made to be a 2x 10k resistor across Vcc and ground and the 
   * sensor should be in the middle of the bridge.
   * */
  *(voltage_mV+VOLTAGE_REAL) = ( ((*(adcSum+SENS_VREF) / totalSamples ) / ADC_RESOLUTION ) * ( VCC_VOLTAGE * 1000.0 ) ) * 2   ;
    
  // Convert raw value to millivolts (mV)
  // LM35 outputs 10 mV per degree Celsius (e.g., 250mV = 25°C)
  
  /* Ideal method 
   *                            Avg. Collected Data
   *                                                 Bit smp.
   *                                                           Voltage Ref
   * float tensionMillivolts = (valeurMoyenneADC   / 4095.0) * 2048.0;
   * */
  *(voltage_mV+SENS_LM35_1) = ((float)( ( *(adcSum+SENS_LM35_1) / totalSamples) ) / ADC_RESOLUTION ) * *(voltage_mV+VOLTAGE_REAL) ;
	
	*(voltage_mV+SENS_LM35_2) = ((float)( ( *(adcSum+SENS_LM35_2) / totalSamples) ) / ADC_RESOLUTION ) * *(voltage_mV+VOLTAGE_REAL) ;
  
  /* Calculate temperature in Celsius
   * LM35 give 10 mV for 1°C, so : temperature = voltage (mV) / 10
   * */
  *(fTemp+SENS_LM35_2) = *(voltage_mV+SENS_LM35_2) / 10.0 ; 
  
  *(fTemp+SENS_LM35_1) = *(voltage_mV+SENS_LM35_1) / 10.0;
  
  // Convert Celsius to Fahrenheit
  *(fTemp+SENS_LM35_1_FARENHEIGHT) = (*(fTemp+SENS_LM35_1) * 1.8) + 32.0;
  
  /*
   * This DrawSection draw only the tft.fillRect(...) where almost all
   * are BLACK to only erase the variable printed at screen . 
   * */
  DrawSection( DRAW_FILLRECT ) ; 
  
  /* A2 Section for LM35-2, Voltage */
	DrawSection( DRAW_VAR, 11, String( String( *(voltage_mV+SENS_LM35_2) ) + " mV" ).c_str() ) ;
	
  /* A2 Section for LM35-2, Temperature */
	DrawSection( DRAW_VAR, 13, String( String( *(fTemp+SENS_LM35_2) )+ " C" ).c_str() ) ;

  /* ADC Section */
  DrawSection( DRAW_VAR, 15, String( (float)( ( *(adcSum+SENS_LM35_1) / totalSamples) ) ).c_str() ) ;
  
  /* Voltage Section for LM35-1 */
  DrawSection( DRAW_VAR, 17, String( String( *(voltage_mV+SENS_LM35_1) ) + " mV " ).c_str() ) ;
  
  /* Temperature Celsius / Farenheight section */
  DrawSection( DRAW_VAR, 19, String( String( *(fTemp+SENS_LM35_1) ) + " C | " + String( *(fTemp+SENS_LM35_1_FARENHEIGHT) ) + " F" ).c_str() ) ;
  
  /* ADC Resolution */
  DrawSection( DRAW_VAR, 21, String( String( ADC_RESOLUTION ) + " bit" ).c_str() ) ;
  
  /* Voltage Reference section .*/
  DrawSection( DRAW_VAR, 23, String( String( *(voltage_mV+VOLTAGE_REAL), 2  ) + " V" ).c_str() ) ;
    
  // Wait 1 second before the next reading
  // Stopping the LM35 Power first, then count GEN_WAIT_VAL
  digitalWrite(vPwr_pin, LOW);
  delay(GEN_WAIT_VAL*5);
  /*Power on of vRef_pin will be upon run of loop before the loop for start accumulating answer. */

}
