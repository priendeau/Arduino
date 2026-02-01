//#include <stdio.h>
#include <Arduino.h>
#include <Scheduler.h>
/*
 * Source information 
 * Used library Version 
 * Scheduler    0.4.4   
 * U8g2         2.34.22 
 * SPI          1.0     
 * Wire         1.0     
 *
 * Used platform Version
 * arduino:samd  1.8.14  
 *
 * Here the full demonstration of the menu_helper started inside esp8266
 * for SerialEventCommander and SerialCommanderPing. Here the version 
 * that include a SerialEvent for usb-cdc SAMD21 know in generic market as 
 * HW-819, which is plain SAMD21 micro-controller.
 * 
 * This include a terminal where you can type upon appearing of CMD>
 * help / about 
 * and many more command. Actually no command rather than help / about
 * and other command are still delivering a Pre-ambule. Don't forget 
 * you may experiencing typing command and having few or no information
 * until you type it twice. It's apparently the baud-rate which this one
 * had been lowered to 38400, but still having no answer. Except command
 * reboot that work, you'll see by typing it once you don't have your 
 * terminal to close, try it again and you'll see. 
 * 
 * And this version include the Looping screen of TempMonitorXeon, while 
 * you do command and the terminal prompt. And some day all the interaction 
 * including changing the information on the screen. Actually the application
 * is testing the loop-text and the menu-helper running simultanously, which 
 * is not bad for a first try. 
 * 
 * 
 * 
 * */



/*
* LCD SPI ST7567 1.8 inch no backlight
  section:
*/

#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#define STEP_DISPLAY_SCROLL   4

U8G2_ST7567_OS12864_1_4W_SW_SPI u8g2(U8G2_R0, 
                              /* clock=*/ 13, 
                              /* data= */ 11, 
                              /* cs=   */  7, 
                              /* dc=   */  9, 
                              /* reset=*/  8 );
/*
* End of section
*/


#if !defined( TERMINAL_BAUD )
#define TERMINAL_BAUD_SPEED         38400
#else
#define TERMINAL_BAUD_SPEED         TERMINAL_BAUD
#endif

#define DEFAULT_SERIAL_TIMEOUT	    150

#define MAJOR_VERSION               1
#define MINOR_VERSION               0000011

#define PWM_RES                     12

#define HOSTNAME                    "SAMDSerialCooler"

#define HOST_COMMAND_GREET          "CMD>"

#define TIMEZONE "EST5EDT"
#define DEFAULT_SLEEP               10000

#define DEBUG_SERIAL_EVENT          1
#define DEBUG_COMMAND_EVALUATION    1

/*
* LCD SPI ST7567 1.8 inch no backlight
  section:
*/
u8g2_uint_t offset;			// current offset for the scrolling text
u8g2_uint_t width;			// pixel width of the scrolling text (must be lesser than 128 unless U8G2_16BIT is defined

/* Template text reference
 * It's possible to change them, but because some pointer-text own __TIME__ __TEMP__ tag
 * they will convert __TIME__ into HH:MM:SS from time device, __TEMP__ from bmp280 
 * and these change occur often as it require a Template and a variable relative to 
 * own the final change. As example textTitleTpl own __TIME__ and __TEMP__ tag as
 * once every second the title is scrolling and will update the __TIME__ and __TEMP__ tag
 * and put everything inside *textTitle and this one will be show in the Display device.
 * */
char *textTitleTpl       = " XeonServer2650 __TIME__ __TEMP__ ";	// scroll this text from right to left
char *textUnderScrollTpl = "Avg: __VALUE__" ;
char *textSecndTpl       = "Ref.T.: __VALUE__" ;
char *textTimeTpl        = "Time: HH:MM:SS" ;
char *textPumpTpl        = "Pump : __MSG__" ;
/*
 * Variable where Template-text reference will be deposed.
 * */
char *textTitle                = textTitleTpl ;
char *textUnderScroll          = textUnderScrollTpl ;
char *textSecnd                = textSecndTpl ;
char *textTime                 = textTimeTpl ;
char *textPump                 = textPumpTpl ;

const uint8_t* SetFontName = u8g2_font_inb16_mr ;
//const uint8_t* SetFontNameSmall = u8g2_font_courR10_tf ; 
const uint8_t* SetFontNameSmall = u8g2_font_pressstart2p_8f ;

/*
* End of section
*/

String StrReadTerm = ""; 
String StrCmdRecv = "" ;

bool bStateEndCmdIn = false ;

const int uart_buffer_size = (1024 * 2);

uint_least16_t  longPwmRes  = PWM_RES;

typedef enum enum_ExprCmd 
{
  CMD_NONE            = 0,
  CMD_HELP            = 1,
  CMD_ABOUT           = 2,
  CMD_VERSION         = 3,
  CMD_SET_TIME        = 4,
  CMD_GET_TIME        = 5,
  CMD_SET_TITLE       = 6,
  CMD_SET_TITLE_TUS   = 7,
  CMD_SET_TITLE_SCND  = 8,
  CMD_SET_TITLE_TIM   = 9,
  CMD_SET_TITLE_PUMP  = 10,
  CMD_UPDATE_TIME     = 11,
  CMD_GET_TEMP        = 12,
  CMD_SET_PUMP_LVL    = 13,
  CMD_GET_PUMP_LVL    = 14,
  CMD_SET_COOLING_LVL = 15,
  CMD_GET_COOLING_LVL = 16,
  CMD_SET_UNDER_CLV   = 17,
  CMD_SET_OVER_CLV    = 18,
  CMD_GET_COOLING_RNG = 19,
  CMD_REBOOT          = 20,
  CMD_SAVE            = 21,
  CMD_LOAD            = 22,
  CMD_UNKNOW          = 23,
  CMD_END			        = 24
} ExprCmd ;

typedef enum enum_ExprCmdStatus 
{
  CMD_NEW             = 0 ,
  CMD_REQUIRE_DATA    = 1 ,
  CMD_NOTHING_REQ     = 2 ,
  CMD_INCOMPLETE_DATA = 3 ,
  CMD_TERMINATED      = 4 
} ExprCmdStatus ;

typedef struct st_CmdToName 
{ 
	ExprCmd     enumType ;
  const char  *chNameCmd  ;  
} stCmd_t ;

typedef struct st_HelperExprInfo
{
  ExprCmdStatus CmdStatus = CMD_NEW  ;
  ExprCmd       CmdType   = CMD_NONE ;
  char          *chMsgOut ;
} st_HelperInfo ; 

stCmd_t TableCommand[] = { {CMD_HELP,            "help"                } ,
                           {CMD_ABOUT,           "about"               } ,
                           {CMD_VERSION,         "version"             } ,
                           {CMD_SET_TIME,        "set time"            } ,
                           {CMD_GET_TIME,        "get time"            } ,
                           {CMD_SET_TITLE,       "set title"           } ,
                           {CMD_SET_TITLE_TUS,   "set title under"     } ,
                           {CMD_SET_TITLE_SCND,  "set title second"    } ,
                           {CMD_SET_TITLE_TIM,   "set title time"      } ,
                           {CMD_SET_TITLE_PUMP,  "set title pump"      } ,
                           {CMD_UPDATE_TIME,     "update time"         } ,
                           {CMD_GET_TEMP,        "get temp"            } ,
                           {CMD_SET_PUMP_LVL,    "set pump power"      } ,
                           {CMD_SET_COOLING_LVL, "set cooling power"   } ,
                           {CMD_SET_PUMP_LVL,    "get pump power"      } ,
                           {CMD_SET_COOLING_LVL, "get cooling power"   } ,
                           {CMD_SET_UNDER_CLV,   "set lowest cooling"  } ,
                           {CMD_SET_OVER_CLV,    "set highest cooling" } ,
                           {CMD_GET_COOLING_RNG, "get cooling range"   } ,
                           {CMD_REBOOT,          "reboot"              } ,
                           {CMD_SAVE,            "save conf"           } ,
                           {CMD_LOAD,            "load conf"           } ,                           
                           {CMD_END,             "END"                 } 
                          } ;

st_HelperInfo stMainHelperInformation;

/*
* LCD SPI ST7567 1.8 inch no backlight
  section:
*/
void ScreenScrollText()
{
  u8g2_uint_t intX ;
  
  u8g2.firstPage();
  do 
  {
  
    // draw the scrolling text at current offset
    intX = offset;
    u8g2.setFont(SetFontName);		              // set the target font
    do 
    {								                            // repeated drawing of the scrolling text...
      u8g2.drawUTF8(intX, 17, textTitle);		    // draw the scolling text
      intX += width ;						                // add the pixel width of the scrolling text
    } while( intX < u8g2.getDisplayWidth() );		// draw again until the complete display is filled
    
    u8g2.setFont(SetFontNameSmall);		
    u8g2.setCursor(0, 29);            
    u8g2.print(textUnderScroll);	     
    u8g2.setFont(SetFontNameSmall);
    u8g2.setCursor(0, 39);
    u8g2.print( textSecnd );
    u8g2.setFont(SetFontNameSmall);
    u8g2.setCursor(0, 49);
    u8g2.print(textTime) ;
    u8g2.setFont(SetFontNameSmall);
    u8g2.setCursor(0, 59);
    u8g2.print( textPump ) ; 

  } while ( u8g2.nextPage() );
  
  offset-=STEP_DISPLAY_SCROLL;
  							// scroll by one pixel
  if ( (u8g2_uint_t)offset < (u8g2_uint_t)-width )
  {
    offset = 0;							// start over again
  }	 
  yield();
}

/*void UnderTitleAnnouncing( )
{
    u8g2.setFont(SetFontNameSmall);		
    u8g2.setCursor(0, 29);            
    u8g2.print(textUnderScroll);	     
    u8g2.setFont(SetFontNameSmall);
    u8g2.setCursor(0, 39);
    u8g2.print( textSecnd );
    u8g2.setFont(SetFontNameSmall);
    u8g2.setCursor(0, 49);
    u8g2.print(textTime) ;
    u8g2.setFont(SetFontNameSmall);
    u8g2.setCursor(0, 59);
    u8g2.print( textPump ) ; 
}*/

/*
* End of section
*/


/* Apparently a reset function. Used to reboot the SAMD21 micro-controller 
   and test the load / save configuration to boot and collect to a specific
   flash-region the last configration saved. This function enforce a reset.
*/
//void(* resetFunc) (void) = 0;

ExprCmd	CommandEvaluation( )
{
	stCmd_t ItemCmd ;
	ExprCmd enumExOut = CMD_UNKNOW ; 
	bool isFound = false ;
	
	ItemCmd = TableCommand[0] ;
	int iCount=0 ;
	String StrRightPair ;
	while( ItemCmd.enumType !=  CMD_END )
	{
		//StrLeftPair.reserve( sizeof(ItemCmd.chNameCmd ) );
    StrRightPair.reserve( ( uint )(strlen(ItemCmd.chNameCmd ))+1 );
    StrRightPair.trim() ; 
		StrRightPair=ItemCmd.chNameCmd ; 		
		if( !isFound )
		{ 
			if( StrCmdRecv == StrRightPair )
			{
				isFound=true ;
				enumExOut = ItemCmd.enumType ;  
			}
		}
		iCount++ ; 
		ItemCmd = TableCommand[iCount] ;
	}
	//Serial.printf("  CommandEvaluation: returned number for inspected command : %i\r\n", iCount ) ;

	if( enumExOut == CMD_UNKNOW )
	{
		SerialUSB.println("  CommandEvaluation: Hit unknow command." ) ;	
	}
#if defined( DEBUG_COMMAND_EVALUATION )
	SerialUSB.print("  Return enum-element id: "+String( (int)(enumExOut) )+"\r\n" ) ;
#endif
	return enumExOut ; 
}

void serialEventManual() 
{
	char chSerialRead ;
  if( !bStateEndCmdIn )
  {
    while ( SerialUSB.available() )
    {
      chSerialRead = (char)( SerialUSB.read() );
      if (  chSerialRead == '\r'  ) 
      {
#if defined(DEBUG_SERIAL_EVENT)
        SerialUSB.print("\r\nserialEventManual: \r\n  command received:["+String( StrReadTerm )+"], length:"+String( StrReadTerm.length() )+"\r\n" ) ;
#endif
        if( StrReadTerm.length() > 0 )
        {
          bStateEndCmdIn = true;
          StrReadTerm.trim() ; 
          StrCmdRecv = StrReadTerm ;
          StrReadTerm = "" ;  
        }
      }
      else
      {
        StrReadTerm += chSerialRead;
      }
    }
  }
  yield();
}

void helper_menu( st_HelperInfo &stHelperOut )
{
  //st_HelperInfo stHelperOut ; 
  //ExprCmd HashCmd = getCommand( StrStreamCmd ); 
  String StrMsgRet ; 
#if defined( DEBUG_HELPER_MENU )
	SerialUSB.print("\r\nMenu helper get the command:[ "+ StrCmdRecv +" ]\r\n" ) ;
#endif
  //ExprCmd ecStateSwitch = CommandEvaluation( StrStreamCmd ) ;  
  switch( stHelperOut.CmdType )
  {
	  case CMD_HELP :
      stHelperOut.CmdStatus = CMD_NOTHING_REQ ;
      StrMsgRet="Help Menu for the application SAMDSerialCooler\r\nInitially the application is accessible through the serial with a\r\n minimum configuration where you can change it depend of your thermal\r\n specification of you CPU or cooled device where this micro-controller\r\nis equiped to control with an L298N motor driver dedicated to control\r\nthe pump and PWM and mofset and couple of temperature device to manage\r\ncooling of the device. \r\nHere several choice to make it possible to change setting and obtain\r\ninformation for the current running cooler.\r\n\r\nMenu option:\r\nhelp                 This help.\r\nabout                About this application.\r\nversion              Print version of the application.\r\nget time             Report time from the SAMD micro-controller.\r\nset time             Set time manually and update DS130X i2c device.\r\nupdate time          Caliber SAMD timer and extract time from i2c dev.\r\nset title            Change the Big Title in Display.\r\nset title under      Change the Title under the Scrolling title.\r\nset title second     Change the Title second under the Scrolling one.\r\nset title time       Change the Title-Time section.\r\nset title pump       Change the  Title of pump section..\r\nget temp             Report temperature for some device.\r\nget pump power       Get Pump PWM level information.\r\nset pump power       Set Pump PWM level information.\r\nget cooling power    Get the cooling overall PWM level.\r\nset cooling power    Set the cooling overall PWM level.\r\nset lowest cooling   Set lowest temperature to not exceed.\r\nset highest cooling  Set highest temperature to not exceed.\r\nget cooling range    Report actual limit for lowest/highest temp.\r\nreboot               Reboot the SAMD microntroller and stop the cooling.\r\nsave conf            Save the configuration inside flash.\r\nload conf            Load the configuration from the flash.\r\n";
    break;

	  case CMD_ABOUT :
      stHelperOut.CmdStatus = CMD_NOTHING_REQ ;
      StrMsgRet="You reach the about.\r\nThis application is designed for cooling a Xeon processor out of its\r\narea. It's from a genuine based bios-like micro-controller that was\r\nchosen to control efficiently Xeon thermal dissipation from 105 Watts\r\nto over 145 Watts. As limits from family of E5-26XXv4 are not hotter\r\nthan 150 watts, it has been chosen to step a cooling from 2 sets of\r\npelletier and a 30 to 60 watt pump. All  are in DC and are operating\r\nat 12 volts. It's possible to load configuration from a set of\r\nconfiguration. Designed to prevent formation of water in case of\r\nover-cooling, it's designed to load several configuration to correspond\r\nto summer, fall, winter, spring or even garage  to prevent leakage.\r\nThese settings and installation are not install with heater, for winter\r\n garage or even space area.\r\n";
		break;

    case CMD_VERSION :
      stHelperOut.CmdStatus = CMD_NOTHING_REQ ;
      StrMsgRet="SAMDSerialEvent Version " + String(MAJOR_VERSION) +"."+ String(MINOR_VERSION)+ "\r\n";
    break;

    case CMD_SET_TIME :
      stHelperOut.CmdStatus = CMD_REQUIRE_DATA;
      StrMsgRet="Set time manually and update present DS130X i2c device.\r\n";
    break ;
    
    case CMD_SET_TITLE:
      stHelperOut.CmdStatus = CMD_REQUIRE_DATA;
      StrMsgRet="Change the big-title of the Display device.\r\nTemplate title:["+ String(textTitleTpl)+"]\r\nNew Title:";
    break ;

    case CMD_SET_TITLE_TUS:
      stHelperOut.CmdStatus = CMD_REQUIRE_DATA;
      StrMsgRet="Change the title under the scrolling title of the Display device.\r\nTemplate title:["+ String(textUnderScrollTpl)+"]\r\nNew Title:";
    break ;
    
    case CMD_SET_TITLE_SCND:
      stHelperOut.CmdStatus = CMD_REQUIRE_DATA;
      StrMsgRet="Change the second title under the scrolling title of the Display device.\r\nTemplate title:["+ String(textSecndTpl)+"]\r\nNew Title:";
    break ;

    case CMD_SET_TITLE_TIM:
      stHelperOut.CmdStatus = CMD_REQUIRE_DATA;
      StrMsgRet="Change the time-title of the Display device.\r\nTemplate title:["+ String(textTimeTpl)+"]\r\nNew Title:";
    break ;

    case CMD_SET_TITLE_PUMP:
      stHelperOut.CmdStatus = CMD_REQUIRE_DATA;
      StrMsgRet="Change the pump title of the Display device.\r\nTemplate title:["+ String(textPumpTpl)+"]\r\nNew Title:";
    break ;

    case CMD_GET_TIME :
      stHelperOut.CmdStatus = CMD_NOTHING_REQ;
      StrMsgRet="Report the time from micro-controller.\r\n";
    break ;
    
    case CMD_UPDATE_TIME :
      stHelperOut.CmdStatus = CMD_NOTHING_REQ ;
      StrMsgRet="SAMD21 micro-controller equiped from DS130X i2c will update time from this device.\r\n";
    break ;
    
    case CMD_GET_TEMP :
      stHelperOut.CmdStatus = CMD_NOTHING_REQ ;
      StrMsgRet="Report all temperature, internal-cooled, external-air.\r\n";
    break ;
    
    case CMD_SET_PUMP_LVL :
      stHelperOut.CmdStatus = CMD_REQUIRE_DATA;
      StrMsgRet="Set the pump PWM level ( 0 to "+String( (2^longPwmRes)-1 )+" )\r\n";
    break ;
    
    case CMD_GET_PUMP_LVL :
      stHelperOut.CmdStatus = CMD_NOTHING_REQ;
      StrMsgRet="Report Actual Pump PWM level.\r\n";
    break ;
    
    case CMD_SET_COOLING_LVL :
      stHelperOut.CmdStatus = CMD_REQUIRE_DATA;
      StrMsgRet="Set the Cooling PWM level ( 0 to "+String( (2^longPwmRes)-1 )+" )\r\n";
    break ;
    
    case CMD_SET_UNDER_CLV :
      stHelperOut.CmdStatus = CMD_REQUIRE_DATA;
      StrMsgRet="Set the lowest temperature the cooler can reach before pausing or stabilizing the temperature.\r\n";
    break ;
    
    case CMD_SET_OVER_CLV :
      stHelperOut.CmdStatus = CMD_REQUIRE_DATA;
      StrMsgRet="Set the highest temperature the cooler can reach before taking action to cool-down to a targeted temperature.\r\n";
    break ;

    case CMD_GET_COOLING_RNG :
      stHelperOut.CmdStatus = CMD_NOTHING_REQ;
      StrMsgRet="Report Lowest and Highest limit set for temperature.\r\n";
    break ;

    case CMD_SAVE :
      stHelperOut.CmdStatus = CMD_NOTHING_REQ ;
      StrMsgRet="Save the actual setting including highest/lowest temperature and PWM level for pump and cooler.\r\n";
    break ;
    
    case CMD_LOAD :
      stHelperOut.CmdStatus = CMD_NOTHING_REQ ;
      StrMsgRet="Load the setting from flash and apply it directly.\r\n";
    break ;
		
		case CMD_REBOOT :
      stHelperOut.CmdStatus = CMD_TERMINATED;
      __disable_irq(); 
      NVIC_SystemReset(); 
      //resetFunc();
      while (true); 
       
    break ;
		
    case CMD_UNKNOW :
      stHelperOut.CmdStatus = CMD_TERMINATED;
      //stHelperOut.CmdType   = CMD_UNKNOW;
			//isCmdFound=false ; 
			StrMsgRet="Unknow command:[" + String(StrCmdRecv) + "].\r\n" ; 
		break;
	}
  
  if( stHelperOut.CmdStatus == CMD_INCOMPLETE_DATA )
  {
#if defined( DEBUG_HELPER_MENU )    
    SerialUSB.print("\r\nRequest with incomplete data are simply closed.\r\nEnd of command.\r\n");
#endif
    stHelperOut.CmdStatus = CMD_NEW ;
    bStateEndCmdIn=false ;  
    StrCmdRecv="" ; 
  }

  size_t iLength = strlen( StrMsgRet.c_str() ) + 1 ; 
  stHelperOut.chMsgOut = ( char* )malloc( iLength * sizeof(char) ) ;
  strcpy(stHelperOut.chMsgOut, (char*)(StrMsgRet.c_str() ));
}

void MenuAction( st_HelperInfo &stMainInfo )
{
  switch( stMainInfo.CmdType )
  {
    case CMD_SET_TIME :
      if( stMainInfo.CmdStatus == CMD_REQUIRE_DATA )
      {
        SerialUSB.print("Time enter: " + StrCmdRecv + "\r\n" );
      }
      stMainInfo.CmdStatus = CMD_TERMINATED ; 
    break ;

    case CMD_SET_TITLE:
      if( stMainInfo.CmdStatus == CMD_REQUIRE_DATA )
      {
        SerialUSB.print("New Title:[ " + StrCmdRecv + "]\r\n" );
        
        size_t iLength = strlen( StrCmdRecv.c_str() ) + 1 ; 
        textTitleTpl = ( char* )malloc( iLength * sizeof(char) ) ;
        strcpy(textTitleTpl, (char*)(StrCmdRecv.c_str() ));
        textTitle=textTitleTpl;
      }
      stMainInfo.CmdStatus = CMD_TERMINATED ; 
    break;
    
    case CMD_SET_TITLE_TUS:
      if( stMainInfo.CmdStatus == CMD_REQUIRE_DATA )
      {
        SerialUSB.print("New Title:[ " + StrCmdRecv + "]\r\n" );
        
        size_t iLength = strlen( StrCmdRecv.c_str() ) + 1 ; 
        textUnderScrollTpl = ( char* )malloc( iLength * sizeof(char) ) ;
        strcpy(textUnderScrollTpl, (char*)(StrCmdRecv.c_str() ));
        textUnderScroll=textUnderScrollTpl;
      }
      stMainInfo.CmdStatus = CMD_TERMINATED ; 
    break;
    
    case CMD_SET_TITLE_SCND:
      if( stMainInfo.CmdStatus == CMD_REQUIRE_DATA )
      {
        SerialUSB.print("New Title:[ " + StrCmdRecv + "]\r\n" );
        
        size_t iLength = strlen( StrCmdRecv.c_str() ) + 1 ; 
        textSecndTpl = ( char* )malloc( iLength * sizeof(char) ) ;
        strcpy(textSecndTpl, (char*)(StrCmdRecv.c_str() ));
        textSecnd=textSecndTpl;
      }
      stMainInfo.CmdStatus = CMD_TERMINATED ; 
    break;
    
    case CMD_SET_TITLE_TIM:
      if( stMainInfo.CmdStatus == CMD_REQUIRE_DATA )
      {
        SerialUSB.print("New Title:[ " + StrCmdRecv + "]\r\n" );
        
        size_t iLength = strlen( StrCmdRecv.c_str() ) + 1 ; 
        textTimeTpl = ( char* )malloc( iLength * sizeof(char) ) ;
        strcpy(textTimeTpl, (char*)(StrCmdRecv.c_str() ));
        textTime=textTimeTpl;
      }
      stMainInfo.CmdStatus = CMD_TERMINATED ; 
    break;
    
    case CMD_SET_TITLE_PUMP:
      if( stMainInfo.CmdStatus == CMD_REQUIRE_DATA )
      {
        SerialUSB.print("New Title:[ " + StrCmdRecv + "]\r\n" );
        
        size_t iLength = strlen( StrCmdRecv.c_str() ) + 1 ; 
        textPumpTpl = ( char* )malloc( iLength * sizeof(char) ) ;
        strcpy(textPumpTpl, (char*)(StrCmdRecv.c_str() ));
        textPump=textPumpTpl;
      }
      stMainInfo.CmdStatus = CMD_TERMINATED ; 
    break;
    
    case CMD_SET_PUMP_LVL :
      if( stMainInfo.CmdStatus == CMD_REQUIRE_DATA )
      {
        SerialUSB.print("Pump level entry: " + StrCmdRecv + "\r\n" );
      }
      stMainInfo.CmdStatus = CMD_TERMINATED ; 
    break ;

    case CMD_SET_COOLING_LVL :
      if( stMainInfo.CmdStatus == CMD_REQUIRE_DATA )
      {
        SerialUSB.print("Cooling factor: " + StrCmdRecv + "\r\n" );
      }
      stMainInfo.CmdStatus = CMD_TERMINATED ; 
    break ;

    case CMD_SET_UNDER_CLV :
      if( stMainInfo.CmdStatus == CMD_REQUIRE_DATA )
      {
        SerialUSB.print("Minimum temperature targeted: " + StrCmdRecv + "\r\n" );
      }
      stMainInfo.CmdStatus = CMD_TERMINATED ; 
    break ;

    case CMD_SET_OVER_CLV :
      if( stMainInfo.CmdStatus == CMD_REQUIRE_DATA )
      {
        SerialUSB.print("\r\nMaximum temperature targeted: " + StrCmdRecv + "\r\n" );
      }
      stMainInfo.CmdStatus = CMD_TERMINATED ; 
    break ;

    case CMD_LOAD :
      if( stMainInfo.CmdStatus == CMD_NOTHING_REQ )
      {
        SerialUSB.println("Restoring information" );
      }
      stMainInfo.CmdStatus = CMD_TERMINATED ; 
    break;

    case CMD_SAVE :
      if( stMainInfo.CmdStatus == CMD_NOTHING_REQ )
      {
        SerialUSB.println("Saving information." );
      }
      stMainInfo.CmdStatus = CMD_TERMINATED ; 
    break;

    case CMD_GET_TEMP :
      if( stMainInfo.CmdStatus == CMD_NOTHING_REQ )
      {
        SerialUSB.println("Temperature report:" );
      }
      stMainInfo.CmdStatus = CMD_TERMINATED ; 
    break;

    case CMD_UPDATE_TIME :
      if( stMainInfo.CmdStatus == CMD_NOTHING_REQ )
      {
        SerialUSB.println("Updating time." );
      }
      stMainInfo.CmdStatus = CMD_TERMINATED ; 
    break;    
    
  }
  if( stMainInfo.CmdStatus == CMD_TERMINATED )
  {
    SerialUSB.print("\r\nEnd of command.\r\n");
    stMainInfo.CmdStatus = CMD_NEW ; 
  }
  
}

void ConditionalGreet( st_HelperInfo &stMainHelper )
{
  if( stMainHelper.CmdStatus != CMD_REQUIRE_DATA ) 
  {
    SerialUSB.print( HOST_COMMAND_GREET );
  }
}

void ActionHandler()
{
  /*serialEventManual() ; 
   serialEventManual() was moved in Scheduler.startLoop( serialEventManual )
   since Scheduler.h is involved, it must complete an valid entry ending serialEventManual
   entry and while this function ActionHandler does filter bStateEndCmdIn to complete
   and treat a valid entry ActionHandler will alway call yield() at the end than 
   serialEventManual
  */
  if ( bStateEndCmdIn ) 
	{
    if ( stMainHelperInformation.CmdStatus == CMD_NEW )
    {
      //stMainHelperInformation.CmdType = CommandEvaluation( StrCmdRecv ) ;
      stMainHelperInformation.CmdType = CommandEvaluation( ) ;
      //helper_menu( stMainHelperInformation, StrCmdRecv ) ;
      helper_menu( stMainHelperInformation ) ;
      /* 
        Forced to print-out information. First message from help_menu 
        does provide some hint and do have to be printed before 
        launching MenuAction. Example of to be printed before doing
        any other action is the CMD_DEL_HOST where if the list of 
        TableHostIP is not empty will create a list 1-to-N where the 
        answer will be recover over the SerialEvent and own the 
        selection inside StrCmdRecv. 
      */
      SerialUSB.print( stMainHelperInformation.chMsgOut ) ;
      /* Some command are leaving the helper_menu in 
      stMainHelperInformation.CmdStatus == CMD_NOTHING_REQ to 
      have post message, but most of them are good to pass in 
      stMainHelperInformation.CmdStatus in CMD_NEW we are 
      doing here for having MenuAction working for all CMD_REQUIRE_DATA.
      and this test will enforce the CMD_NEW
      */
      //if (stMainHelperInformation.CmdStatus == CMD_NOTHING_REQ )
      //{
      //  stMainHelperInformation.CmdStatus = CMD_NEW ; 
      //}
      if ( stMainHelperInformation.CmdStatus == CMD_REQUIRE_DATA )
      {
        bStateEndCmdIn=false ;  
        StrCmdRecv="" ; 
      }
      switch( stMainHelperInformation.CmdStatus ) 
      {
        case CMD_NOTHING_REQ :
        case CMD_TERMINATED :
          //ConditionalGreet( stMainHelperInformation ) ;
          //MenuAction( stMainHelperInformation, StrCmdRecv ) ;
          MenuAction( stMainHelperInformation ) ;
          bStateEndCmdIn=false ;  
          StrCmdRecv="" ; 
        break ; 
      }
    }
    else
    {
      if ( stMainHelperInformation.CmdStatus == CMD_REQUIRE_DATA )
      {
        ConditionalGreet( stMainHelperInformation ) ;
        //MenuAction( stMainHelperInformation, StrCmdRecv ) ;
        MenuAction( stMainHelperInformation ) ;
      }
      /* Reset everything inside stMainHelperInformation.
      */
      free( stMainHelperInformation.chMsgOut ) ;
      stMainHelperInformation.CmdStatus = CMD_NEW  ; 
      stMainHelperInformation.CmdType   = CMD_NONE ;

      /* Put bStateEndCmdIn to false let the next serialEventManual
        start cumulating char into the StrReadTerm. With esp8266 
        it has no effect until serialEventManual is call, on ESP32
        apparently start when the terminal is ready to read data 
        and require the bStateEndCmdIn to become true to tell there
        is new command to treat.
      */
      bStateEndCmdIn=false ;  
      StrCmdRecv="" ; 

      //}
      
    }
    ConditionalGreet( stMainHelperInformation ) ;
  }
  yield();
}

void setup() 
{
/*
* LCD SPI ST7567 1.8 inch no backlight
  section:
*/
  u8g2.begin();
  u8g2.clearBuffer();
  //u8g2.setFont(u8g2_font_inb30_mr);	// set the target font to calculate the pixel width
  u8g2.setFont( SetFontName) ;
  //u8g2.setFont( u8g2_font_inr16_mn ) ; 
  width = u8g2.getUTF8Width(textTitle);		// calculate the pixel width of the text
  //width = width * 2 ;
  u8g2.setContrast(0);
  u8g2.setFontMode(0);		// enable transparent mode, which is faster
  Scheduler.startLoop( ScreenScrollText ); 
/*
* End of section
*/

  StrReadTerm.reserve( uart_buffer_size/8 );
  SerialUSB.begin(TERMINAL_BAUD_SPEED);
  while ( !SerialUSB )
  {
    delay(10);
  }
  //SerialUSB.setTimeout( DEFAULT_SERIAL_TIMEOUT ) ;

  delay(DEFAULT_SLEEP);
  SerialUSB.print("\r\n") ; 
  SerialUSB.println( "BOOTING" );
  SerialUSB.println("SAMD Serial Event for server-cooler.");
  SerialUSB.println( String( HOSTNAME ) + " Version " + String(MAJOR_VERSION) +"."+ String(MINOR_VERSION)+ "\r\n");
  SerialUSB.print( HOST_COMMAND_GREET );
  
  Scheduler.startLoop( ActionHandler );
}

void loop() 
{
  serialEventManual();
  //ActionHandler();
  delay(10) ; 
}
