
#define ASTROLAB_DECIMAL        1
#define ASTROLAB_HUM_DECIMAL    1
#define ASTROLAB_BP_DECIMAL     2

#define GFX_NO_XBITMAP          1
#define GFX_NO_GRAY_BITMAP      1
#define GFX_NO_RGB_BITMAP       1
#define GFX_NO_DRAW_UI          1
#define GFX_NO_CANVAS8          1
#define GFX_NO_CANVAS16         1
#define GFX_NO_GRAPHICS         1
#define GFX_NO_HELPER           1
//#define GFX_NO_ALL_5X7FONT    1

#define USING_DHT11_ECU         1
#define DHT11_MAX_LOOP          500
#define DHT11_SLEEP_INTERVAL    15 
#define DHT11_LOOP_TIMER        120000
#define USING_GY91_ECU          1
#define USING_OLED_SSD1306      1
#define NOT_SPI_DEVICE          1

/*
You can un-comment SERIAL_MSG to let the Serial.print
be effective, it was wrap around with define to allow 
to save space in memory of MCU .  
*/
//#define SERIAL_MSG              1

#define PRESSURE_MSG            "PRESSURE (KPA):_P_\nTEMP:_T_ DHT:_DT__SPH_HUM:_DH_% ND:_D_\nCP:_X_,_Y_,_Z_"
#define DHT_CALMSG              "DHT11/22 CALIBRATION\nREAD PERIOD(MSEC):\nTEMP:_NB1_ HUM:_NB2_."


#ifdef USING_OLED_SSD1306
#define SCREEN_WIDTH            128 // OLED display width, in pixels
#define SCREEN_HEIGHT           32 // OLED display height, in pixels
#define OLED_RESET              -1
#define SCREEN_ADDRESS          0x3C

#ifdef USING_DHT11_ECU
#include "DHT.h"
#define DHTPIN                  3
#define DHTTYPE                 DHT11
#define OE_LEVEL_CONVERTER_PIN  4
#endif

#define CONV_INCHHG_PASCAL      3386.389000000074
#define CONV_PASCAL_INCHHG      0.00029529983071445
#define CONV_INCH_MM            25.400002697664

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//#include "Wire.h"
#include <Adafruit_GFX.h>
#endif 

/* Required by both MPU9250_GY91 and MPU9250_asukiaaa 
 * ---> Adafruit_BMP280.h
*/
#include <Adafruit_BMP280.h>

#ifdef USING_GY91_ECU
#include <MPU9250_GY91.h>
#else
#include <MPU9250_asukiaaa.h>
#endif 


#ifdef USING_DHT11_ECU
DHT dht(DHTPIN, DHTTYPE);
#endif 

#ifdef USING_OLED_SSD1306
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire /*, OLED_RESET*/); 
#endif 

#ifdef USING_GY91_ECU
Adafruit_BMP280 BoschBMP; // I2C
MPU9250_GY91 MpuSensor;
#else
MPU9250_asukiaaa MpuSensor;
#endif  

float   tPressure, mX, mY, mZ, mDirection, dhTemp, dhHum ;
long    lSensorLoop, lHumWaitRead, lTempWaitRead ; 
int     iExit ; 
String  StrMsg ;
void*   vName = NULL ;  

void setup() 
{

  lSensorLoop=1; 
  lHumWaitRead=0 ; 
  lTempWaitRead=0 ; 

  //StrUUID=String( (uint32_t)&GUID0 ) +String("-")+ String( (uint32_t)&GUID1 )+String("-")+ String( (uint32_t)&GUID2 ); 
  
  Serial.begin(9600);
  delay(500);
#ifdef SERIAL_MSG
  Serial.println(F("BOOT"));
#endif  
  // put your setup code here, to run once:

 /*On Wemos XI it come with a blue light it can be good 
 to change it's state to ON/OFF. This case it offer to 
 turn it off */
 pinMode( LED_BUILTIN , OUTPUT);
 digitalWrite( LED_BUILTIN, LOW );

//#ifdef USING_DHT11_ECU
/*
  Uses of D3 for reading data from Data pin of DHT11 from 
  level converter like TSX0108E level converted with OE pin
  to enable or disable the chip itself. We can put VCCA with 
  OE to leave the chip all pin at HIGH to WOrk or plug the 
  OE to Digital Pin 4 (D4) and call it here in HIGH mode
  it replace the wire . 
*/
  //pinMode( DHTPIN , INPUT);
  //pinMode( OE_LEVEL_CONVERTER_PIN , INPUT_PULLUP);
  //digitalWrite( OE_LEVEL_CONVERTER_PIN, HIGH);
//#endif


  /* **************************
  OLED Screen setup 
  ***************************** */


#ifdef USING_OLED_SSD1306
  if( ! display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) 
  {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
#endif

#ifdef _ESP32_HAL_I2C_H_ // For ESP32
  Wire.begin(SDA_PIN, SCL_PIN);
  MpuSensor.setWire(&Wire);
#else
  Wire.begin();
  MpuSensor.setWire(&Wire);
#endif
  
  //display.display() ;
  delay(2000);
  //display.display(); 

#ifdef USING_OLED_SSD1306
  //display.display() ; 
  display.clearDisplay();
  display.setTextSize(1);
#endif

 
#ifdef SERIAL_MSG
  Serial.println(F("OK"));
#endif 

  /*
  GY-91 Setup 
  */
#ifdef USING_GY91_ECU
  BoschBMP.begin() ; 
  BoschBMP.setSampling( Adafruit_BMP280::MODE_NORMAL,
                        Adafruit_BMP280::SAMPLING_X8,
                        Adafruit_BMP280::SAMPLING_X8,
                        Adafruit_BMP280::FILTER_X2,
                        Adafruit_BMP280::STANDBY_MS_63);
  /*
  MODE_SLEEP
  MODE_FORCED
  MODE_NORMAL
  MODE_SOFT_RESET_CODE

  SAMPLING_NONE
  SAMPLING_X1
  SAMPLING_X2
  SAMPLING_X4
  SAMPLING_X8
  SAMPLING_X16

  FILTER_OFF
  FILTER_X2
  FILTER_X4
  FILTER_X8
  FILTER_X16
  
  STANDBY_MS_1
  STANDBY_MS_63
  STANDBY_MS_125
  STANDBY_MS_250
  STANDBY_MS_500
  STANDBY_MS_1000
  STANDBY_MS_2000
  STANDBY_MS_4000
  
  
  */
  MpuSensor.beginAccel();
  MpuSensor.beginGyro();
  MpuSensor.beginMag();
#endif


#ifdef USING_DHT11_ECU
  dht.begin();
#endif

}


void loop() 
{
  
  StrMsg = PRESSURE_MSG ;
  tPressure = BoschBMP.readPressure() ; 
  if ( (MpuSensor.magUpdate() == 0) )
  {
    mX = MpuSensor.magX();
    mY = MpuSensor.magY();
    mZ = MpuSensor.magZ();
    mDirection = MpuSensor.magHorizDirection(); 
  }

  //dhTemp = 0.00 ; 
  //dhHum = 0.00 ; 
  /*
    Sometime DHT22 / DHT11 report NAN for both Temperature and humidity.
    I do made a while statement because apparently DHT22 does report humidity
    a bit slower around 35%-37% of humidity and running fine until mid december
    start to be dry . If it try to do another NAN for either dhTemp or dhHum
    I shoukd move the object DHT into pointer and delete it if I have a another
    NAN result and respawn it into new instance.  
    So in case NaN is reporting it should be filtered with isnan( ... )
  */
  
  
  lSensorLoop=1 ; 
  iExit = 0 ; 
  
  while( iExit != 1 )
  {
    
    if( lTempWaitRead == 0 )
    {
      dhTemp = dht.readTemperature() ;
      
      if( isnan(dhTemp ) )
      {
       /*DHT11_LOOP_TIMER*/
       delay(lSensorLoop * DHT11_SLEEP_INTERVAL);
       lSensorLoop+=1;
      }
      else
      {
        /* 
         This case imply having one read from dht.readTemperature
         and having (lSensorLoop * DHT11_SLEEP_INTERVAL) <= *DHT11_LOOP_TIMER /  
        */ 
        iExit=1 ;
        lTempWaitRead=( lSensorLoop * DHT11_SLEEP_INTERVAL ) ; 
      }
      /*lSensorLoop >= DHT11_MAX_LOOP*/
      if( (lSensorLoop * DHT11_SLEEP_INTERVAL) >= DHT11_LOOP_TIMER )
      {
       iExit=1; 
       StrMsg=DHT_CALMSG ;
       lTempWaitRead=( lSensorLoop * DHT11_SLEEP_INTERVAL ) ; 
      }
    }
    else
    {
     iExit=1 ;
     dhTemp = dht.readTemperature() ; 
     delay( lTempWaitRead ) ; 
    }
#ifdef SERIAL_MSG
  Serial.println("dht.readTemperature: " + String( dhTemp ));
#endif 
  }
 
  lSensorLoop=1 ;
  iExit = 0 ; 
  while( iExit != 1 )
  {
    if( lHumWaitRead == 0 )
    {
      dhHum = dht.readHumidity() ;
      if( isnan(dhHum) )
      {
        /*DHT11_LOOP_TIMER*/
       delay(lSensorLoop * DHT11_SLEEP_INTERVAL);
       lSensorLoop+=1;
      }
      else
      {
        iExit=1 ; 
        lHumWaitRead=(lSensorLoop * DHT11_SLEEP_INTERVAL) ;
      }
      /*lSensorLoop >= DHT11_MAX_LOOP*/
      if( (lSensorLoop * DHT11_SLEEP_INTERVAL) >= DHT11_LOOP_TIMER   )
      {
       iExit=1; 
       StrMsg=DHT_CALMSG ;
       lHumWaitRead=(lSensorLoop * DHT11_SLEEP_INTERVAL) ;
      }
    }
    else
    {
      iExit=1 ;
      dhHum = dht.readHumidity() ;
      delay( lHumWaitRead ) ; 
    }
#ifdef SERIAL_MSG
  Serial.println("dht.readHumidity: " + String( dhHum ));
#endif 
  }

#ifdef SERIAL_MSG
  Serial.println("lSensorLoop exit with nb loop: " + String(lSensorLoop ));
#endif 

  /* lSensorLoop >= DHT11_MAX_LOOP */
  if( iExit == 1 )
  { 
    if( (lSensorLoop * DHT11_SLEEP_INTERVAL) >= DHT11_LOOP_TIMER )
    {
      StrMsg=DHT_CALMSG ;   
      /* 
       *  Showing the period required to obtain no NaN from the DHT reading
      */
      StrMsg.replace(String("_NB1_"), String( lTempWaitRead  ) ) ;
      StrMsg.replace(String("_NB2_"), String( lHumWaitRead ) ) ;
      BoschBMP.setSampling( Adafruit_BMP280::MODE_NORMAL,
                            Adafruit_BMP280::SAMPLING_X8,
                            Adafruit_BMP280::SAMPLING_X8,
                            Adafruit_BMP280::FILTER_X2,
                            Adafruit_BMP280::STANDBY_MS_2000);
      delay(2500) ; 
    
    }
  
  }
  /*
    This section format StrMsg allowing to space 
    element likesymbol humidity (Hum) is chopped 
    when ASTROLAB_DECIMAL == 1, so it correct 
    thems. 
  */
  if (ASTROLAB_DECIMAL == 1)
  {
    StrMsg.replace(String("_SPH_"), "   " ) ;  
  } 
  if (ASTROLAB_DECIMAL == 2)
  {
    StrMsg.replace(String("_SPH_"), "  " ) ;  
  } 
  
  StrMsg.replace(String("_P_"), String( tPressure/1000, ASTROLAB_BP_DECIMAL ) ) ; 
  StrMsg.replace(String("_T_"), String( BoschBMP.readTemperature(), ASTROLAB_DECIMAL ) ) ;
  StrMsg.replace(String("_X_"), String( mX , ASTROLAB_DECIMAL ) ) ; 
  StrMsg.replace(String("_Y_"), String( mY , ASTROLAB_DECIMAL ) ) ; 
  StrMsg.replace(String("_Z_"), String( mZ , ASTROLAB_DECIMAL ) ) ; 
  StrMsg.replace(String("_D_"), String( mDirection, ASTROLAB_DECIMAL ) ) ; 
  StrMsg.replace(String("_DH_"), String( dhHum, ASTROLAB_HUM_DECIMAL ) ) ; 
  StrMsg.replace(String("_DT_"), String( dhTemp, ASTROLAB_DECIMAL  ) ) ; 

#ifdef USING_OLED_SSD1306  
  display.setCursor(0, 0);
  display.setTextColor(SSD1306_WHITE);
  display.print( StrMsg );
  display.display() ;
#endif 
  //display.display();
  //display.startscrollleft(0x00, 0x0F);
  //delay(3000);
  //display.stopscroll();
  //display.setTextColor(SSD1306_BLACK);
  //display.print( StrMsg );

#ifdef SERIAL_MSG
  Serial.println(F("DONE"));
#endif 
  delay(500);

#ifdef USING_OLED_SSD1306
  display.clearDisplay();    
  
#endif

}
