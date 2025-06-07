#if !defined( TERMINAL_BAUD )
#define TERMINAL_BAUD_SPEED  115200
#else
#define TERMINAL_BAUD_SPEED  TERMINAL_BAUD
#endif

#include <Arduino.h>
#include "mbedtls/base64.h"

#include "SD.h"
#include "FS.h"

#define DEFAULT_SERIAL_TIMEOUT	150
#define HOSTNAME                "SERIALCOMMANDER"

// Digital I/O used
#define SD_CS          5
#define SPI_MOSI      23  
#define SPI_MISO      19
#define SPI_SCK       18

#define TIMEZONE "EST5EDT"

String StrReadTerm ; 
String StrCmdRecv  ; 

/*
 * The menu in base64 format. 
 * it should give a 1133 character long String once decoded.
 * 
 */
const char* chMsgB64Menu = "VGhlIEhFTFAgU2VjdGlvbi4NCg0KMS0gaGVscA0KCVRoaXMgaXMgdGhpcyBoZWxwLg0KMi0gYWJvdXQNCglUaGlzIGlzIGFuIEFyZHVpbm8gcHJvamVjdCBjb21waWxlZCB3aXRoDQoJQXJkdWlub0lERSAyLjIuMS1hcm02NCBvciBhcmR1aW5vLWNsaQ0KCVZlcnNpb246IDEuMS4xIENvbW1pdDogZmE2ZWFmY2IuIFVzZQ0KCW1vc3RseSBlc3AzMiBsaWJyYXJ5IHZlcnNpb24gMy4wLjUuDQozLSBsaXN0DQoJTGlzdCB0aGUgY29udGVudCBvZiB5b3VyIFNEL1NJUEZGUy9GQVQNCglvbiBlc3AzMiBvciBTRGNhcmQuDQoJU3ludGF4OiBsaXN0IFtTRC9TUElGRlNdDQo0LSBvcGVuDQoJT3BlbiBhIG1lZGlhIG9uIFNEL1NQSUZGUyBkZXZpY2Ugb3INCglTRGNhcmQuDQoJU3ludGF4OiBvcGVuIFtTRC9TUElGRlNdIEZJTEVfTkFNRQ0KNS0gY29ubmVjdA0KCU9wZW4gYW4gVVJJL1VSTCBsb2NhdGlvbiBmb3IgcGxheWluZy4NCglTeW50YXg6IGNvbm5lY3QgW1VSTC9VUkldDQo2LSBzdG9wDQoJU3RvcCBhY3R1YWwgcGxheWluZy4NCglTeW50YXg6IHN0b3ANCjctIHBsYXkNCglTdGFydCBhIHBsYXkgaWYgaXMgc3RvcHBlZC4NCglTeW50YXg6IHBsYXkNCjgtIHdpZmkNCglXaWZpIHNlY3Rpb24sIHRoaXMgaXMgcG9zc2libGUgdG8NCglyZS1jb25maWd1cmUsIHF1ZXJ5IHRoZSB3aWZpIHBhcmFtZXRlcg0KCWFuZCBnZXQgaXQncyBjb25maWd1cmF0aW9uLg0KCVN5bnRheDoNCgl3aWZpIFtvbi9vZmZdIC0gVHVybiBvbi9vZmYgdGhlIHdpZmkuDQoJd2lmaSBjaGFuZ2UgW3NzaWQvcGFzcy9ob3N0XSAtIHNldCBuZXcNCgkJY29ubmVjdGlvbiBvciBjaGFuZ2UgdGhlIGhvc3RuYW1lLg0KCXdpZmkgc2NhbiAtIFNjYW4gd2lmaSBmb3IgQlNTSUQvU1NJRCBuZXR3b3JrLg0KCXdpZmkgcXVlcnkgW2lwL21hYy9kbnNdIC0gb2J0YWluIGluZm9ybWF0aW9ucy4NCjktIHBpbmcNCglQaW5nIGFuIElQIGFkZHJlc3MuDQoJU3ludGF4OiBwaW5nIElQX0FERFINCjEwLSB0ZWxuZXQNCglBY2Nlc3MgdG8gdGVsbmV0IG9yIGNvbW1hbmQgdGVybWluYWwuDQoJU3ludGF4OiB0ZWxuZXQgVVJML0lQX0FERFIgWzAtMTAyNF0NCjExLSByZWJvb3QNCglSZXN0YXJ0IHRoZSBNUDNJMlNURVNMQURSSVZFUiBob3N0Lg0KDQo=";

bool bStateEndCmdIn			 = false ; 

const int uart_buffer_size = (1024 * 2);

typedef enum enum_ExprCmd 
{
	CMD_HELP    = 0,
	CMD_ABOUT   = 1,
	CMD_LIST    = 2,
	CMD_OPEN    = 3,
	CMD_CONNECT = 4,
	CMD_STOP    = 5,
	CMD_PLAY    = 6,
	CMD_WIFI    = 7,
	CMD_PING    = 8,
	CMD_TELNET  = 9,
	CMD_REBOOT  = 10,
	CMD_UNKNOW  = 11,
	CMD_END			= 12
} ExprCmd ;

typedef struct st_CmdToName 
{ 
	ExprCmd enumType ;
  char *chNameCmd  ;  
} stCmd_t;

stCmd_t TableCommand[12] = { {CMD_HELP    , "help"    } , 
		                         {CMD_ABOUT   , "about"   } ,
		                         {CMD_LIST    , "list"    } ,
		                         {CMD_OPEN    , "open"    } ,
		                         {CMD_CONNECT , "connect" } ,
		                         {CMD_STOP    , "stop"    } ,
		                         {CMD_PLAY    , "play"    } ,
		                         {CMD_WIFI    , "wifi"    } ,
		                         {CMD_PING    , "ping"    } ,
		                         {CMD_TELNET  , "telnet"  } ,
		                         {CMD_REBOOT  , "reboot"  } ,
		                         {CMD_END     , "END"     } } ;


#if defined( STRINGCHAR_COMPARE )
bool chChainCompare( char *chPair1, char *chPair2 )
{
	bool isSzCompareId     = true ;
	bool isChCompareId     = true ; 
	bool isReturnIdentical = true ;
	size_t stSizePair1     = sizeof( chPair1 ); 
	size_t stSizePair2     = sizeof( chPair2 );
	char chPairA, chPairB ; 
	Serial.printf("    chChainCompare: PartA:[%s], PartB:[%s], sizeA:[%i], sizeB:[%i]\r\n", chPair1, chPair2, stSizePair1, stSizePair2 ) ;
	
	/*
	 * Comparing the size. 
	 * */
	if( stSizePair1 != stSizePair2)
	{
		isSzCompareId = false ;
	}
	else
	{
		isSzCompareId = true ;
	}
	
	if( isSzCompareId )
	{
		for( int iCount=0 ; iCount <= sizeof(stSizePair1)-1; iCount++ )
		{
			if( !isChCompareId )
			{
				chPairA = *( chPair1 )+iCount ; 
				chPairB = *( chPair2 )+iCount ; 
				Serial.printf("    position by position comparing [PosA:%i, letterA:%c, PosB:%i, letterB:%c ] \r\n",iCount ,chPairA , iCount, chPairB  ) ;
				if( chPairA != chPairB )
				{
					isChCompareId = false ; 
				}
			}
		}
	}
	else
	{
		isChCompareId = false ;
	}
	
	if( !isChCompareId && !isSzCompareId )
	{
		isReturnIdentical = false ;
	}
	
	if( !isChCompareId && isSzCompareId )
	{
		isReturnIdentical = false ;
	}
	
	return isReturnIdentical ; 
}
#endif 

//ExprCmd	CommandEvaluation( char* chCmdRecv )
ExprCmd	CommandEvaluation( String StrCmdRecv )
{
	stCmd_t ItemCmd ;
	ExprCmd enumExOut = CMD_UNKNOW ; 
	bool isFound = false ;
	
	ItemCmd = TableCommand[0] ;
	int iCount=0 ;
	String StrLeftPair ;
	while( ItemCmd.enumType !=  CMD_END )
	{
		StrLeftPair.reserve( sizeof(ItemCmd.chNameCmd ) );
		StrLeftPair=ItemCmd.chNameCmd ; 		
		//Serial.printf("  CommandEvaluation: StrCmdRecv set to:%s\r\nInspecting key/pair: [string:%s, id:%i]\r\n", StrCmdRecv, StrLeftPair, ItemCmd.enumType ) ;
		if( !isFound )
		{ 
			//(char *)( StrCmdRecv ) == (char *)(ItemCmd.chNameCmd) 
			//if( chChainCompare( StrCmdRecv, StrLeftPair ) == true )
			if( StrCmdRecv == StrLeftPair )
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
		Serial.println("  CommandEvaluation: Hit unknow command." ) ;	
	}
	Serial.printf("  Return enum-element id: %i\r\n", (int)(enumExOut) ) ;
	return enumExOut ; 
}

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() 
{
	char chSerialRead ;
	if( Serial.available() )
	{
		chSerialRead = (char)Serial.read();
		if (  chSerialRead == '\n'  ) 
		{
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

//String helper_menu( char *chStreamCmd )
String helper_menu( String StrStreamCmd )
{
  unsigned char* StrOutMenu ;
  //ExprCmd HashCmd = getCommand( StrStreamCmd ); 
  String StrMsgRet ; 
  unsigned char *chMenuMsg ; 
	Serial.printf("Menu helper get the command:[ %s ]\r\n", StrStreamCmd ) ;
  bool isCmdFound = false ;
  //trim( StrStreamCmd ) ;
  ///ExprCmd ecStateSwitch = CommandEvaluation( (char *)( StrStreamCmd ) ) ;
  ExprCmd ecStateSwitch = CommandEvaluation( StrStreamCmd ) ; 
  switch( ecStateSwitch )
  {
		case CMD_HELP :
			isCmdFound=true ; 
			//StrMsgRet=StrOutMenu;
			//StrOutMenu = (unsigned char*)( malloc( sizeof( char ) * 1133 ) ) ; 
			size_t outlen;
			//(unsigned char*)( StrMsgRet.c_str() )
			chMenuMsg = (unsigned char*)( malloc( ( sizeof(char) * 1133 )+1 ) ) ;
			mbedtls_base64_decode( chMenuMsg, 
														 1133, 
														 &outlen, 
														 (const unsigned char*)( chMsgB64Menu ) , 
														 ( sizeof(chMsgB64Menu) ) ) ;
			StrMsgRet.reserve( 1133 ) ; 
			//StrMsgRet=String( chMenuMsg );
			StrMsgRet = "" + String( (const char*)(chMenuMsg) ) ; 
			free(chMenuMsg) ;
			break;

		case CMD_ABOUT :
			isCmdFound=true ; 
			StrMsgRet="You reach about.\r\n\r\n\r\nThis is an Arduino project compiled with.\r\nArduinoIDE 2.2.1-arm64 or arduino-cli\r\nVersion: 1.1.1 Commit: fa6eafcb. Use\r\nmostly esp32 library version 3.0.5.\r\nAdapted for esp-wrover or wroom 32Mbit\r\nor the N16R8 with PSRAM. It's an internet\r\nrelated player with SD Card capability.\r\nOther services offered here including\r\nretreiving shoutcast music list it was\r\nentirely tested with ATR, or Amsterdams\r\nTrance music.\r\n";
			break;

		case CMD_LIST :
			isCmdFound=true ; 
			StrMsgRet="list";
			break;

		case CMD_OPEN :
			isCmdFound=true ; 
			StrMsgRet="open";
			break;

		case CMD_CONNECT :
			isCmdFound=true ; 
			StrMsgRet="connect";
			break;
		
		case CMD_STOP :
			isCmdFound=true ; 
			StrMsgRet="stop";
			break;
		
		case CMD_PLAY :
			isCmdFound=true ; 
			StrMsgRet="play";
			break;
		
		case CMD_WIFI :
			isCmdFound=true ; 
			StrMsgRet="wifi";
			break;

		case CMD_PING :
			isCmdFound=true ; 
			StrMsgRet="ping";
			break;
		
		case CMD_TELNET :
			isCmdFound=true ; 
			StrMsgRet="telnet";
			break;

		case CMD_REBOOT :
			isCmdFound=true ; 
			StrMsgRet="reboot";
			break;
	
		case CMD_UNKNOW : 
			isCmdFound=false ; 
			StrMsgRet="Unknow command:[" + String(StrStreamCmd) + "].\r\n" ; 
			break;
	}
  return StrMsgRet ; 
}


void setup() 
{
	/* Allow 256 byte in reserve for StrReadTerm, apparently it overflow 
	 * every time a += occur to the string and does not add space for. 
	 * SerialStringReader */
	
	StrReadTerm.reserve( uart_buffer_size/8 );

  delay(5000) ;
	
  Serial.begin(TERMINAL_BAUD_SPEED);
  while ( ! Serial ) 
  { 
		delay(5) ; 
	}
	
	Serial.setTimeout( DEFAULT_SERIAL_TIMEOUT ) ; 
  Serial.println( "BOOTING" );
  Serial.println("Serial Commander");
  Serial.print("CMD>");
  
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  SPI.setFrequency(1000000);
  
  SD.begin(SD_CS);
  

	delay(1500) ;
}

void loop()
{

	/*
	 * More code to come. 
	 * 
	 * */

	if ( bStateEndCmdIn ) 
	{
		Serial.println( helper_menu( StrCmdRecv ) ) ;
		bStateEndCmdIn=false ;  
		StrCmdRecv="" ; 
		Serial.print("CMD>");
	}

	/*
	 * even more there's too. 
	 * 
	 * */


}
