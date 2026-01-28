
/*
 * This is the Screen widget to test SAMD21 micro-controller with open-smart
 * 1.8 inch monitor with no back-light
 * here the pin connection between SAMD21 micro-controller (HW-819) and ST7567.
 * Micro-Controller     ST7567
 *       13        --->   SCK
 *       11        --->   SDI
 *       9         --->   DC
 *       8         --->   RST/RESET
 *       7         --->   CS
 *       GND       --->   GND
 *       3V3       --->   3V3
 * 
 * This is for testing the display only, you should have one big title 
 * being scroll from right to left and 4 line under with 4 category:
 * line 1  |   BIG TITLE
 * line 2  |
 * line 3  | Avg: __VALUE__
 * line 4  | Ref.T.: __VALUE__
 * line 5  | Time: HH:MM:SS
 * line 6  | Pump: __MSG__
 * 
 * and the "BIG TITLE" is scrolling . It's only this part because inside 
 * SAMDSerialEvent.ino it include the SerialEvent and the menu_helper
 * where you can interact with a terminal connected usuallt in /dev/ttyACM0.
 * Note. Is't a CDC-USB port and everything is writed with SerialUSB, see 
 * SAMDSerialEvent.ino for the integration of the Screen menu on this 
 * open-smart 1.8 inch monitor, and a terminal to see the helper.
*/

#include <Arduino.h>
#include <U8g2lib.h>

#include <Scheduler.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

//U8G2_ST7567_PI_132X64_1_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 7, /* dc=*/ 9, /* reset=*/ 8);  // Pax Instruments Shield, LCD_BL=6
//U8G2_ST7567_PI_132X64_1_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 7, /* dc=*/ 9, /* reset=*/ 8);  // Pax Instruments Shield, LCD_BL=6

//U8G2_ST7567_OS12864_1_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 7, /* dc=*/ 9, /* reset=*/ 8);

#define STEP_DISPLAY_SCROLL   4

/*U8G2_ST7567_OS12864_1_4W_SW_SPI u8g2(U8G2_R0, 
                               clock= 13, 
                               data=  11, 
                                 cs=   7, 
                                 dc=   9,
                                 reset=8 );
*/
U8G2_ST7567_OS12864_1_4W_SW_SPI u8g2(U8G2_R0, 
                              /* clock=*/ 13, 
                              /* data= */ 11, 
                               /* cs=  */  7, 
                               /* dc=  */  9, 
                               /* reset=*/ 8 );

/*U8G2_ST7567_JLX12864_1_4W_SW_SPI u8g2(U8G2_R0, 
                                /* clock= 13, 
                                 /* data= 11, 
                                    /* cs= 7, 
                                    /* dc= 9, 
                                 /* reset= 8);
*/

//U8G2_ST7567_JLX12864_1_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 7, /* dc=*/ 9, /* reset=*/ 8);  
//U8G2_ST7567_122X32_1_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 7, /* dc=*/ 9, /* reset=*/ 8);  
//U8G2_ST7567_122X32_1_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 7, /* dc=*/ 9, /* reset=*/ 8);  
//U8G2_ST7567_OS12864_1_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 7, /* dc=*/ 9, /* reset=*/ 8);  
//U8G2_ST7567_OS12864_1_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 7, /* dc=*/ 9, /* reset=*/ 8);  
//U8G2_ST7567_ENH_DG128064_1_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8); 
//U8G2_ST7567_ENH_DG128064_1_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8); 
//U8G2_ST7567_ENH_DG128064I_1_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8); 
//U8G2_ST7567_ENH_DG128064I_1_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8); 
//U8G2_ST7567_64X32_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); 
//U8G2_ST7567_HEM6432_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); 
//U8G2_ST7567_ERC13232_1_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8); 
//U8G2_ST7567_ERC13232_1_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8); 

u8g2_uint_t offset;			// current offset for the scrolling text
u8g2_uint_t width;			// pixel width of the scrolling text (must be lesser than 128 unless U8G2_16BIT is defined

const char *text = " XeonServer2650 __TIME__ __TEMP__ ";	// scroll this text from right to left
const char *textUnderScroll = "Avg: __VALUE__" ;
const char *textSecnd = "Ref.T.: __VALUE__" ;
const char *textTime  = "Time: HH:MM:SS" ;
const char *textPump = "Pump : __MSG__" ;

const uint8_t* SetFontName = u8g2_font_inb16_mr ;
//const uint8_t* SetFontNameSmall = u8g2_font_courR10_tf ; 
const uint8_t* SetFontNameSmall = u8g2_font_pressstart2p_8f ;

void ScreenScrollText()
{
  u8g2_uint_t intX ;
  
  u8g2.firstPage();
  do 
  {
  
    // draw the scrolling text at current offset
    intX = offset;
    u8g2.setFont(SetFontName);		// set the target font
    do 
    {								// repeated drawing of the scrolling text...
      u8g2.drawUTF8(intX, 17, text);			// draw the scolling text
      intX += width ;						// add the pixel width of the scrolling text
    } while( intX < u8g2.getDisplayWidth() );		// draw again until the complete display is filled
    
    UnderTitleAnnouncing( ) ; 
  } while ( u8g2.nextPage() );
  
  offset-=STEP_DISPLAY_SCROLL;
  							// scroll by one pixel
  if ( (u8g2_uint_t)offset < (u8g2_uint_t)-width )
  {
    offset = 0;							// start over again
  }	 
  yield();
}

void UnderTitleAnnouncing( )
{
    u8g2.setFont(SetFontNameSmall);		//Set a smaller font.
    u8g2.setCursor(0, 29);            //Locate under the the scroll line.
    u8g2.print(textUnderScroll);	    // Show the first steady line in title : value 
    u8g2.setFont(SetFontNameSmall);
    u8g2.setCursor(0, 39);
    u8g2.print( textSecnd );
    u8g2.setFont(SetFontNameSmall);
    u8g2.setCursor(0, 49);
    u8g2.print(textTime) ;
    u8g2.setFont(SetFontNameSmall);
    u8g2.setCursor(0, 59);
    u8g2.print( textPump ) ; 
}

void setup() 
{
  u8g2.begin();
  u8g2.clearBuffer();
  //u8g2.setFont(u8g2_font_inb30_mr);	// set the target font to calculate the pixel width
  u8g2.setFont( SetFontName) ;
  //u8g2.setFont( u8g2_font_inr16_mn ) ; 
  width = u8g2.getUTF8Width(text);		// calculate the pixel width of the text
  //width = width * 2 ;
  u8g2.setContrast(0);
  u8g2.setFontMode(0);		// enable transparent mode, which is faster
  Scheduler.startLoop( ScreenScrollText );
}


void loop() 
{
  // put your main code here, to run repeatedly:

  delay(1) ; 

}
