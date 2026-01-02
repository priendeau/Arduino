#include <cstdlib>
#include <cstring>

#include <Pinger.h>
#include <ESP8266WiFi.h>
#include <Arduino.h>

extern "C"
{
  #include <lwip/icmp.h> // needed for icmp packet definitions
  //#include "/home/maxiste/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/tools/sdk/lwip2/include/lwip/icmp.h"
}
/*
  Library required : 
  Using library ESP8266-ping at version 2.0.1  
  Using library ESP8266WiFi at version 1.0 
  the library <lwip/icmp.h> should be inside the current esp8266 sdk inside Arduino.
  This example do use a version after 3.1.2 of the core of esp8266 .

*/
#define MAJOR_VERSION   1
#define MINOR_VERSION   2423547


#define MAX_HOST_ADD  256

#if !defined( TERMINAL_BAUD )
#define TERMINAL_BAUD_SPEED     115200
#else
#define TERMINAL_BAUD_SPEED     TERMINAL_BAUD
#endif

/* DEBUG-SECTION
   UN-COMMENT one of the DEBUG define and you going to get 
   debug from this function.
*/

//#define DEBUG_COMMAND_EVALUATION    1
//#define DEBUG_SERIAL_EVENT          1
//#define DEBUG_ADD_IP                1
//#define DEBUG_HELPER_MENU           1
//#define NO_PING_TEST                1


/* Different DEFAULT_SERIAL_TIMEOUT between esp32 and esp8266 
   Because serialEvent is not compatible with esp8266 there is 
   no possible way to enter character by character and make the 
   menu receiving the backspace function if you made an error.
   And because the serialEvent in esp32 is like a permanent 
   reader using it's own Interrupt at the moment you type a 
   character is block everything and let you type. And also
   if a function run and your seria is available you can type
   while the function helper_menu is treating a value ended with
   \n. In case of esp8266 you have few-seconds like to enter 
   something and because character \n is not punched it will not
   run helper_menu and while helper_menu is running whatever your
   terminal is locked and even with echo the esp8266 will not 
   consider character unless is the turn of serialEventManual.
*/
#if defined(ARDUINO_ARCH_ESP32)
#define DEFAULT_SERIAL_TIMEOUT	150
#elif defined(ARDUINO_ARCH_ESP8266)
#define DEFAULT_SERIAL_TIMEOUT	1500
#endif 

#define HOSTNAME                "SERIALCOMMANDERPING"

#define HOST_COMMAND_GREET      "CMD>"

#define TIMEZONE "EST5EDT"
#define DEFAULT_SLEEP           10000

/*
  Belong to esp8266 documentation, serialEvent is not working and 
  suggest to develop alike funtion and call it directly from loop.
  For this a variable in the main-body StrReadTerm will hold the 
  data filtered inside serialEventManual equivalent of serialEvent.
  Variable StrCmdRecv will hold the command typed at the serial 
  until StrReadTerm read a carriage-return '\n' and put the whole
  command inside StrCmdRecv. 
*/
String StrReadTerm = ""; 
String StrCmdRecv = "" ; 

String StrSSID = ""; 
String StrPass = "";

bool bStateEndCmdIn			 = false ; 
bool stationConnected    = false ;

const int uart_buffer_size = (1024 * 2);

typedef enum enum_ExprCmd 
{
  CMD_NONE            = 0,
	CMD_HELP            = 1,
	CMD_ABOUT           = 2,
	CMD_LIST_HOST       = 3,
	CMD_ADD_HOST        = 4,
	CMD_DEL_HOST        = 5,
	CMD_WIFI_SSID       = 6,
  CMD_WIFI_PASS       = 7,
  CMD_WIFI_CONNECT    = 8,
	CMD_PING            = 9,
  CMD_TEST            = 10,
  CMD_REBOOT          = 11,
  CMD_REBOOT_UPGRADE  = 12,
  CMD_IP_INFO         = 13,
  CMD_UNKNOW          = 14,
	CMD_END			        = 15
} ExprCmd ;

/*
  typedef enum enum_ExprCmdStatus
  Category CMD_REQUIRE_DATA :
   - In case Command fall into CMD_ADD_HOST, CMD_DEL_HOST, 
   CMD_WIFI_SSID, CMD_WIFI_PASS it require data from the 
   user like one entry (IP Address or username or SSIS or 
   host listed from 1-N )
   - In case Command fall into CMD_INCOMPLETE_DATA it's 
   clearly fall into CMD_WIFI_CONNECT without having fill
   the CMD_WIFI_SSID, CMD_WIFI_PASS and held different 
   case not only-one entry. 
   - In a case like CMD_UNKNOW for unknow entry from a 
   serial.read it fall into CMD_TERMINATED and similar
   case for CMD_LIST_HOST, and CMD_DEL_HOST when there is 
   no host listed. And possibly CMD_ADD_HOST will also 
   own a case of CMD_TERMINATED when it reach the maximum
   amount of host to PING, for having not enough memory to 
   add another host (NOT implemented).
   - In case CMD_NOTHING_REQ the case of CMD_PING, CMD_TEST,
   CMD_WIFI_CONNECT, CMD_HELP, CMD_ABOUT will have nothing
   required afte helper_menu have establish one of them and 
   will pass the hand to print the content cumulated inside
   st_HelperInfo.chMsgOut 
*/
typedef enum enum_ExprCmdStatus 
{
  CMD_NEW             = 0 ,
  CMD_REQUIRE_DATA    = 1 ,
  CMD_NOTHING_REQ     = 2 ,
  CMD_INCOMPLETE_DATA = 3 ,
  CMD_TERMINATED      = 4 
} ExprCmdStatus ; 

/* Required to generate the array of command and it's decimal relation
   once the user relate the command from the prompt into command insde 
   array.
*/
typedef struct st_CmdToName 
{ 
	ExprCmd     enumType ;
  const char  *chNameCmd  ;  
} stCmd_t ;

/* Once the user had composed a command the information are 
   store inside this structure and own it's command requirement
   like much information to carry. It's also owning the 
   user-message to send on the terminal. Some message are part 
   message and data need to be provided will start with ':' 
   instead of "CMD>", and thus CmdStatus variable will also
   own the enum related to CMD_REQUIRE_DATA. 
*/
typedef struct st_HelperExprInfo
{
  ExprCmdStatus CmdStatus = CMD_NEW  ;
  ExprCmd       CmdType   = CMD_NONE ;
  char          *chMsgOut ;
} st_HelperInfo ; 


typedef struct st_IpSubLvl 
{ 
	uint  subNet_A ; 
  uint  subNet_B ; 
  uint  subNet_C ; 
  uint  subNet_D ; 
} stIp_Addr_t ;

stIp_Addr_t *TableHostIP = NULL ; 
uint iTableIndex = 0 ;

/*
  Don't provide information for CMD_NONE, CMD_UNKNOW. Some says it's bogus 
  to type 'END' and get END but there is nothing everywhere to manage END.
  In fact it fall into CMD_UNKNOW when the command is not in this list and 
  CMD_NEW is when it's initialized in the main for stMainHelperInformation,
  and at the end of MenuAction the stMainHelperInformation.CmdStatus goes
  back to CMD_NEW.
*/

stCmd_t TableCommand[14] = { {CMD_HELP           , "help"        } , 
		                         {CMD_ABOUT          , "about"       } , 
		                         {CMD_LIST_HOST      , "list host"   } , 
		                         {CMD_ADD_HOST       , "add host"    } , 
		                         {CMD_DEL_HOST       , "del host"    } , 
		                         {CMD_WIFI_SSID      , "wifi ssid"   } , 
		                         {CMD_WIFI_PASS      , "wifi pass"   } , 
		                         {CMD_WIFI_CONNECT   , "wifi connect"} ,
                             {CMD_IP_INFO        , "ip info"     } , 
		                         {CMD_PING           , "ping"        } , 
                             {CMD_TEST           , "test"        } ,
                             {CMD_REBOOT         , "reboot"      } ,
                             {CMD_REBOOT_UPGRADE , "reboot upgrade"},
		                         {CMD_END            , "END"         } } ;

Pinger pinger;

/* Belong to implementation it should start with 
   st_HelperInfo.CmdStatus == CMD_NEW, while passing by the 
   helper_menu it change into exception like 
   st_HelperInfo.CmdStatus = CMD_REQUIRE_DATA 
   and then the loop() might loop again and not catch 
   everything in the first loop, but sequence will not let
   the st_HelperInfo.CmdStatus enter inside helper_menu with
   something else than CMD_NEW.
   
*/
st_HelperInfo stMainHelperInformation;
              
//ExprCmd	CommandEvaluation( char* chCmdRecv )
//ExprCmd	CommandEvaluation( String StrCmdRecv )
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
		Serial.println("  CommandEvaluation: Hit unknow command." ) ;	
	}
#if defined( DEBUG_COMMAND_EVALUATION )
	Serial.printf("  Return enum-element id: %i\r\n", (int)(enumExOut) ) ;
#endif
	return enumExOut ; 
}

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
#if defined(ARDUINO_ARCH_ESP32)
void serialEvent() 
{
	char chSerialRead ;
	if( Serial.available() )
	{
		chSerialRead = (char)( Serial.read() );
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
#elif defined(ARDUINO_ARCH_ESP8266)
void serialEventManual() 
{
	char chSerialRead ;
  if( !bStateEndCmdIn )
  {
    while ( Serial.available() )
    {
      chSerialRead = (char)( Serial.read() );
      if (  chSerialRead == '\r'  ) 
      {
#if defined(DEBUG_SERIAL_EVENT)
        Serial.printf("\r\nserialEventManual: \r\n  command received:[%s], length:%i\r\n", StrReadTerm.c_str() ,StrReadTerm.length() ) ;
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
}
#endif

void IpList(String &StrMsgRet )
{
  /* There is an official Index-related to the TableHostIP and using it 
  to see if there is IP to ping. Variable iTableIndex will be increased everytime 
  there is an addition and upon deletion it feed the free TableHostIP-location 
  and put the next IP into the deleted one and next IP into previously after 
  deleted IP and will re-confirm the state of iTableIndex. So referring to iTableIndex
  is safe. 
  */
  if( iTableIndex == 0 )
  {
    StrMsgRet=StrMsgRet + "\r\nEmpty list, add somes IP destination before .\r\n" ; 
  }
  else
  {
    StrMsgRet="\r\nGive all the added host to ping.\r\n\r\n";
    int iTableList=0 ; 
    stIp_Addr_t *IpAdd ;
    IpAdd = (stIp_Addr_t*)( malloc( sizeof( stIp_Addr_t ) ) ) ;
    for( iTableList=0 ; iTableList <= iTableIndex-1; iTableList++)
    {
      *IpAdd = *(TableHostIP + iTableList ) ;
      StrMsgRet=StrMsgRet + String(iTableList) + " -> IP: " + String( IpAdd->subNet_A ) + "." + String( IpAdd->subNet_B) + "." + String( IpAdd->subNet_C ) + "." + String( IpAdd->subNet_D ) + "\r\n" ; 
    }
  }
}

void AddIp()
{
  String StrToNum ; 
  char cInspect ; 
  unsigned int intIpHost[4] ; 
  int iPos =0 ; 
  stIp_Addr_t *IpAdd ;
  IpAdd = (stIp_Addr_t*)( malloc( sizeof( stIp_Addr_t ) ) ) ;
  
#if defined(DEBUG_ADD_IP)
  Serial.printf("\r\nInspecting entry: String:(%s) length:(%i)\r\n", StrCmdRecv, StrCmdRecv.length()) ;
#endif
  /* Removing exceeding space at the end including carriage return
  */
  StrCmdRecv.trim();
  for( int iLen=0 ; iLen <= StrCmdRecv.length() ; iLen++ )
  {
    cInspect=(char)( StrCmdRecv[iLen] ) ;
#if defined(DEBUG_ADD_IP)
    Serial.printf("\r\nScreening entry: in-subnet:(%i) char:(%s) value:(%i)\r\n", iPos,String(cInspect), (uint)(cInspect)  ) ; 
#endif    
    if( ( (int)(cInspect) != 46 ) && ( (int)(cInspect) != 0 )  )
    {
         StrToNum+=StrCmdRecv[iLen] ;
    }
    else
    {
        if( iPos <= 4 )
        {
#if defined(DEBUG_ADD_IP)
          Serial.printf("\r\nConverting String to Number:sub-net[%i]: %s\r\n", iPos, StrToNum ) ; 
#endif
          intIpHost[iPos]=(unsigned int)(StrToNum.toInt());
          iPos+=1; 
          StrToNum="" ;
        }
    }
  }
  IpAdd->subNet_A = intIpHost[0] ;
  IpAdd->subNet_B = intIpHost[1] ;
  IpAdd->subNet_C = intIpHost[2] ;
  IpAdd->subNet_D = intIpHost[3] ;
  *(TableHostIP+iTableIndex) = *IpAdd ;
  iTableIndex+=1;
  free(IpAdd) ;
}

void DelIp()
{
  /* How DelIp work.
  It should extract the IP list from the TableHostIP and associate it to a new point. 
  While looping in the list, the selected one will not be inserted inside the other
  new pointer list and at the end it assign the new pointer to TableHostIP suppressing
  the one selected for deletion. Also it's required to reduce the index iTableIndex by
  one. The way it work, TableHostIP have no problem to receive a pointer-list from 
  somewhere (this function), and will keep it because TableHostIP is external. Like 
  AddIp does add one by one to the TableHostIP, DelIp is similar but does not link
  the discarded one, which is equivalent to lose it instead putting '0' into subNet_A,
  subNet_B,subNet_C,subNet_D.
  */
  
  stIp_Addr_t *NewTableHostIP = NULL ;
  stIp_Addr_t *IpAdd ; 
  int iRankToRemove = StrCmdRecv.toInt() ; 
  /*Create the other buffer for the new list of stIp_Addr_t.*/
  NewTableHostIP = (stIp_Addr_t*)( malloc( sizeof( stIp_Addr_t ) * MAX_HOST_ADD  ) ) ;

  for( int iTableList=0 ; iTableList <= iTableIndex-1; iTableList++)
  {
    IpAdd = (stIp_Addr_t*)( malloc( sizeof( stIp_Addr_t ) ) ) ;
    *IpAdd = *(TableHostIP + iTableList ) ;
    
    //StrMsgRet=StrMsgRet + String(iTableList) + " -> IP: " + String( IpAdd->subNet_A ) + "." + String( IpAdd->subNet_B) + "." + String( IpAdd->subNet_C ) + "." + String( IpAdd->subNet_D ) + "\r\n" ; 
    //IpAdd->subNet_A, IpAdd->subNet_B, IpAdd->subNet_C, IpAdd->subNet_D    
    if( iTableList != iRankToRemove)
    {
       *(NewTableHostIP + iTableList ) = *IpAdd ; 
    }
    free(IpAdd) ; 
  }    
  /* This free whenever it call it also free a Global
  variabl.Your actual pointer-record disapear.
  After it this internal assignation from NewTableHostIP
  hosting the transfert structure_type into a with a 
  new reference to TableHostIP. 
  */
  free(TableHostIP) ;
  TableHostIP = NewTableHostIP ; 
  /*Decreasing the Global index, iTableIndex by one since 
   we removing only one element at the time.*/
  iTableIndex=iTableIndex-1;
  //free( NewTableHostIP ) ; 
}

//String helper_menu( char *chStreamCmd )
//void helper_menu( st_HelperInfo &stHelperOut, String StrStreamCmd )
void helper_menu( st_HelperInfo &stHelperOut )
{
  //st_HelperInfo stHelperOut ; 
  //ExprCmd HashCmd = getCommand( StrStreamCmd ); 
  String StrMsgRet ; 
#if defined( DEBUG_HELPER_MENU )
	Serial.print("Menu helper get the command:[ "+ StrCmdRecv +" ]\r\n" ) ;
#endif
  //ExprCmd ecStateSwitch = CommandEvaluation( StrStreamCmd ) ;  
  switch( stHelperOut.CmdType )
  {
		case CMD_HELP :
      stHelperOut.CmdStatus = CMD_NOTHING_REQ ;
      //stHelperOut.CmdType   = CMD_HELP;
			//isCmdFound=true ; 
      StrMsgRet="Help Menu for the application SerialCommanderPing.\r\nInitially the application is accessible through the serial\r\nline and does nothing until you specifiy the wifi ssid/password\r\n and add host to ping. Once configured you type ping \r\ncommand to process. There is no saving to restore it later.\r\n\r\n\tList of available command.\r\n\r\nhelp            This help\r\nabout           About this application\r\nlist host       List host added to the Ping\r\nadd host        Add an host to ping-it\r\ndel host        Delete an host from the Ping list\r\nwifi ssid       Set the wifi SSID name to connect to.\r\nwifi pass       Set the wifi password to obtain your IP.\r\nwifi connect    once SSID/PASS defined it's authentificate\r\n                and obtain your ip.\r\nip info         Obtain IP information once connected.\r\nping            Processing all the host to PING.\r\ntest            Launch a generic example including\r\n                ping a fake host.\r\nreboot          Reboot the micro-controller.\r\nreboot upgrade  Reboot into download mode.\r\n\r\n";
    break;

		case CMD_ABOUT :
      stHelperOut.CmdStatus = CMD_NOTHING_REQ ;
      //stHelperOut.CmdType   = CMD_ABOUT;
			//isCmdFound=true ; 
			StrMsgRet="You reach the about.\r\n\r\nThis is an Arduino project compiled with.\r\nArduinoIDE 2.2.1-x86_64  or  arduino-cli\r\nVersion: 1.1.1  Commit: fa6eafcb.  Use\r\nmostly esp32 library version  3.0.5.  For\r\nesp8266  the final  sdk 3.1.2,  and \r\nESP8266-ping version 2.0.1,  ESP8266WiFi \r\ncome from sdk. Adapted  for  esp8266, \r\nnode_mcu or esp32 wroom  32Mbit. It's \r\nan internet PING application to  program \r\na list of  host to ping and  show  relative \r\ninformation to ping. Consult  the  help\r\n to get more command. \r\n\r\n";
		break;

		case CMD_LIST_HOST :
      stHelperOut.CmdStatus = CMD_TERMINATED;
      //stHelperOut.CmdType   = CMD_LIST_HOST;
			//isCmdFound=true ; 
			//StrMsgRet="\r\nGive all the added host to ping.\r\nSize of host list:[" + String((sizeof(TableHostIP)/4)-1) +"]\r\n";
      IpList( StrMsgRet ) ;
		break;

		case CMD_ADD_HOST :
      stHelperOut.CmdStatus = CMD_REQUIRE_DATA;
      //stHelperOut.CmdType   = CMD_ADD_HOST;
			//isCmdFound=true ; 
			StrMsgRet="Add an host to ping :";
      //ReadLineInfo = Serial.readStringUntil('\n');
			//StrMsgRet = StrMsgRet + "\r\nSpecified host: " + ReadLineInfo + "\r\n"; 
    break;

		case CMD_DEL_HOST :
      stHelperOut.CmdStatus = CMD_REQUIRE_DATA;
      //stHelperOut.CmdType   = CMD_DEL_HOST;
			//isCmdFound=true ; 
      IpList( StrMsgRet ) ;
      StrMsgRet=StrMsgRet + "\r\n\r\nHost number to delete:" ;
		break;
		
		case CMD_WIFI_CONNECT :
      stHelperOut.CmdStatus = CMD_NOTHING_REQ;
      //stHelperOut.CmdType   = CMD_WIFI_CONNECT;
      if ( StrSSID == "" ) 
      {
        stHelperOut.CmdStatus = CMD_INCOMPLETE_DATA ;
        StrMsgRet="\r\nNetwork name / SSID was not specified.\r\n"; 
      }
      if ( StrPass == "" ) 
      {
        stHelperOut.CmdStatus = CMD_INCOMPLETE_DATA ;
        StrMsgRet="\r\nPassword was not specified.\r\n"; 
      }
      if( stHelperOut.CmdStatus == CMD_NOTHING_REQ)
      {
        //isCmdFound=true ; 
        StrMsgRet="\r\nEstablish a connection to wifi with provided credential.\r\n";
        // Connect to WiFi access point
        
      }        
    break;

		case CMD_WIFI_SSID :
      stHelperOut.CmdStatus = CMD_REQUIRE_DATA;
      //stHelperOut.CmdType = CMD_WIFI_SSID;
			//isCmdFound=true ; 
			StrMsgRet="\r\nSpecify which SSID/Wifi network name to connect to.\r\nSSID Name:";
		break;

		case CMD_WIFI_PASS :
			stHelperOut.CmdStatus = CMD_REQUIRE_DATA ;
      //stHelperOut.CmdType = CMD_WIFI_PASS;
      //isCmdFound=true ; 
			StrMsgRet="\r\nSpecify the Wifi/SSID password. Warning\r\nit's clear text information and it can\r\nbe seen by anyone :";
		break;

    case CMD_IP_INFO :
      //stHelperOut.CmdType = CMD_IP_INFO ; 
      stHelperOut.CmdStatus = CMD_NOTHING_REQ ;
      if( stationConnected )
      {
        StrMsgRet="\r\nObtain IP information based on current SSID network connection.\r\n" ; 
      }
      else
      {
        stHelperOut.CmdStatus = CMD_INCOMPLETE_DATA ; 
        StrMsgRet="\r\nTo obtain IP information you should connect to a network name first.\r\n" ;
      }
    break ;
		
    case CMD_TEST :
#ifdef NO_PING_TEST 
      stHelperOut.CmdStatus = CMD_TERMINATED ;
      //stHelperOut.CmdType = CMD_TEST;
			//isCmdFound=true ; 
			StrMsgRet="\r\nPing-test was disabled.\r\nRecompile the application\r\nwith no #define NO_PING_TEST\r\n";
#else
      stHelperOut.CmdStatus = CMD_TERMINATED ;
      //stHelperOut.CmdType = CMD_TEST;
			//isCmdFound=true ; 
      if( stationConnected )
      {
        StrMsgRet="\r\nLaunch the default Ping-test\r\n";
        PingTest() ;

      }
      else
      {
        StrMsgRet="\r\nTo start the default Ping test you should connect to a network name first.\r\n" ;
      }
#endif
		break;
		case CMD_PING :
			stHelperOut.CmdStatus = CMD_NOTHING_REQ;
      //stHelperOut.CmdType = CMD_PING;
      //isCmdFound=true ; 
      if( stationConnected )
      {
        StrMsgRet="Do the Ping of all the host.";
        if( iTableIndex == 0 )
        {
          StrMsgRet=StrMsgRet + "\r\nYou haven't provide IP address as ping destination using 'add host'.\r\n" ;
        }
      }
      else
      {
        StrMsgRet="\r\nTo start the Ping of all your host you should connect to a network name first.\r\n" ;
        if( iTableIndex == 0 )
        {
          StrMsgRet=StrMsgRet + "\r\nYou haven't provide IP address as ping destination using 'add host' once with your network connection.\r\n" ;
        }
      }
		break;

		case CMD_REBOOT :
      stHelperOut.CmdStatus = CMD_TERMINATED;
      //stHelperOut.CmdType   = CMD_REBOOT; 
      ESP.restart();
    break ;
		
    case CMD_REBOOT_UPGRADE :
      stHelperOut.CmdStatus = CMD_TERMINATED;
      //stHelperOut.CmdType   = CMD_REBOOT_UPGRADE; 
      ESP.rebootIntoUartDownloadMode() ; 
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
    Serial.print("\r\nRequest with incomplete data are simply closed.\r\nEnd of command.\r\n");
#endif
    stHelperOut.CmdStatus = CMD_NEW ;
    bStateEndCmdIn=false ;  
    StrCmdRecv="" ; 
  }

  size_t iLength = strlen( StrMsgRet.c_str() ) + 1 ; 
  stHelperOut.chMsgOut = ( char* )malloc( iLength * sizeof(char) ) ;
  //stHelperOut.chMsgOut = ( char* )( StrMsgRet.c_str() ) ;
  strcpy(stHelperOut.chMsgOut, (char*)(StrMsgRet.c_str() ));
  //return stHelperOut ;
  //return StrMsgRet ; 
}

#if !defined( NO_MENU_ACTION ) 
//void MenuAction( st_HelperInfo &stMainInfo, String StrStreamCmd )
void MenuAction( st_HelperInfo &stMainInfo )
{
  switch( stMainInfo.CmdType )
  {
    case CMD_ADD_HOST :
      if( stMainInfo.CmdStatus == CMD_REQUIRE_DATA )
      {
        Serial.printf("IP address to add inside host-list: %s\n", StrCmdRecv );
        AddIp() ; 
      }
      stMainInfo.CmdStatus = CMD_TERMINATED ; 
    break ;
    case CMD_DEL_HOST :
      if( stMainInfo.CmdStatus == CMD_REQUIRE_DATA )
      {
        Serial.printf("host-list number to remove: %s\n", StrCmdRecv ); 
        DelIp() ; 
      }
      stMainInfo.CmdStatus = CMD_TERMINATED ;
    break ;
    case CMD_WIFI_SSID :
      if( stMainInfo.CmdStatus == CMD_REQUIRE_DATA )
      {
        Serial.printf("SSID choosed to connect to: %s\n", StrCmdRecv );
        StrSSID = StrCmdRecv ;
      }
      stMainInfo.CmdStatus = CMD_TERMINATED ; 
    break ;
    case CMD_WIFI_PASS :
      if( stMainInfo.CmdStatus == CMD_REQUIRE_DATA )
      {
        Serial.printf("Wifi password used to connect to SSID(%s) : %s\n",StrSSID, StrCmdRecv );
        StrPass = StrCmdRecv ; 
      }
      stMainInfo.CmdStatus = CMD_TERMINATED ; 
    break ;
    case CMD_WIFI_CONNECT :
      if( stMainInfo.CmdStatus == CMD_NOTHING_REQ )
      {
        Serial.printf("\r\nTrying to connect network name: %s.\r\n ", StrSSID );
        stationConnected= WiFi.begin(StrSSID,StrPass);

        // Check if connection errors
        if(!stationConnected)
        {
          Serial.println("Error, unable to connect specified WiFi network.");
        }
        
        // Wait connection completed
        Serial.print("\n\nConnecting to AP...");
        while(WiFi.status() != WL_CONNECTED)
        {
          delay(500);
          Serial.print(".");
        }
        Serial.print("Ok\r\n");
        Serial.printf( "IP address information:[ %s ]\r\n", WiFi.localIP().toString().c_str() );
        Serial.printf( "IP Gateway information: %s\r\n", WiFi.gatewayIP().toString().c_str());
        Serial.printf( "MAC Address : %s\r\n", String( WiFi.macAddress() ).c_str() ) ;
      }
      stMainInfo.CmdStatus = CMD_TERMINATED ;
    break ;
    case CMD_IP_INFO :
      if( stMainInfo.CmdStatus == CMD_NOTHING_REQ )
      {
        Serial.printf( "IP address information:[ %s ]\r\n", WiFi.localIP().toString().c_str() );
        Serial.printf( "IP Gateway information: %s\r\n", WiFi.gatewayIP().toString().c_str());
        Serial.printf( "MAC Address : %s\r\n", String( WiFi.macAddress() ).c_str() ) ;
      }
      stMainInfo.CmdStatus = CMD_TERMINATED ; 
    break ;
    case CMD_PING :
      if( stMainInfo.CmdStatus == CMD_NOTHING_REQ )
      {
        Serial.printf("\r\nProcess all host to a ping command.\r\n\r\n");
        ActionPing() ; 
      }
    break;
  }
  /* Over that switch the second step from helper_menu that require one
  insertion of data led to a final operation which should be done inside
  the switch or falling most in stMainInfo.CmdStatus == CMD_TERMINATED.
  As most of them are in stMainInfo.CmdStatus == CMD_REQUIRE_DATA it 
  come out of the next loop() sequence where the 
  stMainInfo.CmdStatus == CMD_NEW and if the first loop only allow new
  command to provide check to a second request of information it should
  been done in second and goes with incomplete sets of data telling 
  a CMD_TERMINATED and might be ready to fall into a next request to do
  via CMD>. So it also time to set stMainInfo.CmdStatus = CMD_NEW again. 
  */
  if( stMainInfo.CmdStatus == CMD_TERMINATED )
  {
    Serial.print("\r\nEnd of command.\r\n");
    stMainInfo.CmdStatus = CMD_NEW ; 
  }
  
}
#endif

void ActionPing()
{
  int iTableList=0 ; 
  stIp_Addr_t *IpAdd ;
  IpAdd = (stIp_Addr_t*)( malloc( sizeof( stIp_Addr_t ) ) ) ;

  pinger.OnReceive([](const PingerResponse& response)
  {
    if (response.ReceivedResponse)
    {
      Serial.printf("Reply from %s: bytes=%d time=%lums TTL=%d\r\n",
        response.DestIPAddress.toString().c_str(),
        response.EchoMessageSize - sizeof(struct icmp_echo_hdr),
        response.ResponseTime,
        response.TimeToLive);
    }
    else
    {
      Serial.printf("Request timed out.\r\n");
    }

    // Return true to continue the ping sequence.
    // If current event returns false, the ping sequence is interrupted.
    return true;
  });
  
  pinger.OnEnd([](const PingerResponse& response)
  {
    // Evaluate lost packet percentage
    float loss = 100;
    if(response.TotalReceivedResponses > 0)
    {
      loss = (response.TotalSentRequests - response.TotalReceivedResponses) * 100 / response.TotalSentRequests;
    }
    
    // Print packet trip data
    Serial.printf("Ping statistics for %s:\r\n",response.DestIPAddress.toString().c_str());
    Serial.printf("    Packets: Sent = %lu, Received = %lu, Lost = %lu (%.2f%% loss),\r\n",response.TotalSentRequests,response.TotalReceivedResponses,response.TotalSentRequests - response.TotalReceivedResponses,loss);

    // Print time information
    if(response.TotalReceivedResponses > 0)
    {
      Serial.printf("Approximate round trip times in milli-seconds:\r\n");
      Serial.printf("    Minimum = %lums, Maximum = %lums, Average = %.2fms\r\n",response.MinResponseTime,response.MaxResponseTime,response.AvgResponseTime);
    }
    
    // Print host data
    Serial.printf("Destination host data:\r\n");
    Serial.printf("    IP address: %s\n",response.DestIPAddress.toString().c_str());
    
    if(response.DestMacAddress != nullptr)
    {
      Serial.printf("\r\n    MAC address: " MACSTR "\r\n", MAC2STR(response.DestMacAddress->addr));
    }
    if(response.DestHostname != "")
    {
      Serial.printf("\r\n    DNS name: %s\r\n",response.DestHostname.c_str());
    }

    return true;
  });

  for( iTableList=0 ; iTableList <= iTableIndex-1; iTableList++)
  {
    *IpAdd = *(TableHostIP + iTableList ) ;
    //StrMsgRet=StrMsgRet + String(iTableList) + " -> IP: " + String( IpAdd->subNet_A ) + "." + String( IpAdd->subNet_B) + "." + String( IpAdd->subNet_C ) + "." + String( IpAdd->subNet_D ) + "\r\n" ; 
    
    Serial.printf("\n\nPinging host: %i.%i.%i.%i\r\n", IpAdd->subNet_A, IpAdd->subNet_B, IpAdd->subNet_C, IpAdd->subNet_D);
    if(pinger.Ping(IPAddress( IpAdd->subNet_A, IpAdd->subNet_B, IpAdd->subNet_C, IpAdd->subNet_D ) ) == false )
    {
      Serial.println("Error during ping command.");
    }  
    delay(DEFAULT_SLEEP);
  }    
}

#if !defined( NO_PING_TEST ) 
void PingTest()
{
  pinger.OnReceive([](const PingerResponse& response)
  {
    if (response.ReceivedResponse)
    {
      Serial.printf("Reply from %s: bytes=%d time=%lums TTL=%d\r\n",
        response.DestIPAddress.toString().c_str(),
        response.EchoMessageSize - sizeof(struct icmp_echo_hdr),
        response.ResponseTime,
        response.TimeToLive);
    }
    else
    {
      Serial.printf("Request timed out.\r\n");
    }

    // Return true to continue the ping sequence.
    // If current event returns false, the ping sequence is interrupted.
    return true;
  });
  
  pinger.OnEnd([](const PingerResponse& response)
  {
    // Evaluate lost packet percentage
    float loss = 100;
    if(response.TotalReceivedResponses > 0)
    {
      loss = (response.TotalSentRequests - response.TotalReceivedResponses) * 100 / response.TotalSentRequests;
    }
    
    // Print packet trip data
    Serial.printf("Ping statistics for %s:\r\n",response.DestIPAddress.toString().c_str());
    Serial.printf("    Packets: Sent = %lu, Received = %lu, Lost = %lu (%.2f%% loss),\r\n",response.TotalSentRequests,response.TotalReceivedResponses,response.TotalSentRequests - response.TotalReceivedResponses,loss);

    // Print time information
    if(response.TotalReceivedResponses > 0)
    {
      Serial.printf("Approximate round trip times in milli-seconds:\r\n");
      Serial.printf("    Minimum = %lums, Maximum = %lums, Average = %.2fms\r\n",response.MinResponseTime,response.MaxResponseTime,response.AvgResponseTime);
    }
    
    // Print host data
    Serial.printf("Destination host data:\r\n");
    Serial.printf("    IP address: %s\n",response.DestIPAddress.toString().c_str());
    
    if(response.DestMacAddress != nullptr)
    {
      Serial.printf("\r\n    MAC address: " MACSTR "\r\n", MAC2STR(response.DestMacAddress->addr));
    }
    if(response.DestHostname != "")
    {
      Serial.printf("\r\n    DNS name: %s\r\n",response.DestHostname.c_str());
    }

    return true;
  });
  
  // Ping default gateway
  Serial.printf( "\n\nPinging default gateway with IP %s\r\n", WiFi.gatewayIP().toString().c_str());
  if(pinger.Ping(WiFi.gatewayIP()) == false)
  {
    Serial.println("Error during last ping command.");
  }
  
  delay(DEFAULT_SLEEP);
  
  // Ping technologytourist.com
  Serial.printf("\n\nPinging technologytourist.com\r\n");
  if(pinger.Ping("technologytourist.com") == false)
  {
    Serial.println("Error during ping command.");
  }

  delay(DEFAULT_SLEEP);

  // Ping undefinedname
  Serial.printf("\n\nPinging undefinedname\r\n");
  if(pinger.Ping("undefinedname") == false)
  {
    Serial.println("Error during ping command.");
  }

  delay(DEFAULT_SLEEP);

  // Ping esp32s2 host 10.0.0.32 
  Serial.printf("\n\nPinging esp32s2 host 10.0.0.32\r\n");
  if(pinger.Ping(IPAddress(10,0,0,32)) == false)
  {
    Serial.println("Error during ping command.");
  }

  delay(DEFAULT_SLEEP);

  // Ping esp32 host 10.0.0.52 
  Serial.printf("\n\nPinging esp32 host 10.0.0.52\r\n");
  if(pinger.Ping(IPAddress(10,0,0,52)) == false)
  {
    Serial.println("Error during ping command.");
  }

  delay(DEFAULT_SLEEP);


  // Ping invalid ip
  Serial.printf("\n\nPinging invalid ip 1.2.3.4\r\n");
  if(pinger.Ping(IPAddress(1,2,3,4)) == false)
  {
    Serial.println("Error during ping command.");
  }

}
#endif 

void ConditionalGreet( st_HelperInfo &stMainHelper )
{
  if( stMainHelper.CmdStatus != CMD_REQUIRE_DATA ) 
  {
    Serial.print( HOST_COMMAND_GREET );
  }
}


void setup()
{  
  StrReadTerm.reserve( uart_buffer_size/8 );

  TableHostIP = (stIp_Addr_t*)( malloc( sizeof( stIp_Addr_t ) * MAX_HOST_ADD  ) ) ;

  // Begin serial connection at TERMINAL_BAUD_SPEED or 115200 as default baud
  Serial.begin(TERMINAL_BAUD_SPEED);
  Serial.setTimeout( DEFAULT_SERIAL_TIMEOUT ) ; 
  //while ( ! Serial ) 
  //{ 
	//	delay(5) ; 
	//}
  delay(DEFAULT_SLEEP);
  Serial.print("\r\n") ; 
  Serial.println( "BOOTING" );
#if defined(ARDUINO_ARCH_ESP8266)
  Serial.printf( "\r\nESP8266: %lu.%lu\r\n",MAJOR_VERSION,MINOR_VERSION) ; 
  Serial.println( "Boot Version: " + String( ESP.getBootVersion()) );
  Serial.println( "Core Version: " + String( ESP.getCoreVersion()) );
  Serial.println( "  Chip ID   : " + String( ESP.getChipId()) );
  Serial.println( "SDK version : " + String( ESP.getSdkVersion()) );
  Serial.printf("\r\nCPU Frequency : %lu Mhz.\r\n", ESP.getCpuFreqMHz() );
#endif
  //delay(DEFAULT_SLEEP);
  Serial.println("Serial Commander PING");
  Serial.print( HOST_COMMAND_GREET );
}


void loop()
{
#if defined(ARDUINO_ARCH_ESP8266)
  /*serialEventManual will only capture entry from the terminal if the 
  bStateEndCmdIn == False, because after hitting \r inside serialEventManual
  set the bStateEndCmdIn to True. Merely it enter inside the function serialEventManual
  and exit if there is no 'bStateEndCmdIn == False' */
  serialEventManual() ;
#endif
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
      Serial.print( stMainHelperInformation.chMsgOut ) ;
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
  
}
