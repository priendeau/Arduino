//**********************************************************************
//*    Mp3 I2S Tesla Driver with telnet command and serial interface to*                                                                                                    *
//*    talk to audio-decoder and changing stream with command.         *                                                                                          *
//*                                                                    *
//*    Based on example of :                                           *
//*    audioI2S-- I2S audiodecoder for ESP32,                          *
//*                                                                    *
//*                                                                    *
//**********************************************************************
// 
// 
// THE SOFTWARE IS PROVIDED "AS IS" FOR PRIVATE USE ONLY, IT IS NOT FOR 
// COMMERCIAL USE IN WHOLE OR PART OR CONCEPT. FOR PERSONAL USE IT IS 
// SUPPLIED WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
// BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
// PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHOR
// OR COPYRIGHT HOLDER BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE 
// OR OTHER DEALINGS IN THE SOFTWARE .
// 

/*
 * 
  If this MASTER_BACKEND_VALUE is defined to 0 a bracing of 
  #if MASTER_BACKEND_VALUE <= 1 will be use to disable the audioI2S in 
  flavor of compiling and executing the internet and serial interaction 
  services. This will also have a consequence, compiling it without any 
  extra DEFINE command will disable this BACKEND. 
	
	### Enabling the audioI2S and all the API. 
	arduino-cli compile --verbose --build-property 'build.extra_flags=-DMASTER_BACKEND_SWITCH' 
	--build-property 'compiler.cpp.extra_flag=-DMASTER_BACKEND_SWITCH' 
	* --build-path=${ARDUINO_BASE}/tmp ${CURRENT_PROJECT}

	### Disabling the audioI2S and all the API, but keeping serial access 
	* and ultimately the telnet access. 
	arduino-cli compile --verbose --build-path=${ARDUINO_BASE}/tmp ${CURRENT_PROJECT}

	Where : 
	* ${ARDUINO_BASE} -> variable from prompt usually refer to the base of 
	* Sketches Path. Our example show ARDUINO_BASE refer to /home/${USER}/Documents/Arduino/Sketches
	* 
	* ${CURRENT_PROJET} - variable from prompt usually refer to the path 
	* where the project is. Our example show CURRENT_PROJET refet to 
	* 'esp32_nodemcu/Mp3I2sTeslaDriver/Mp3I2sTeslaDriver'
	* 
	* Acessing from prompt to all the path from the root will be used as 
	* ${ARDUINO_BASE}/${CURRENT_PROJECT}
	* refer to /home/${USER}/Documents/Arduino/Sketches/esp32_nodemcu/Mp3I2sTeslaDriver/Mp3I2sTeslaDriver
	* where arduino-cli should either find a sketch.yaml holding the 
	* FQBN for arduino attached board to application. Default esp32 look 
	* like : 'esp32:esp32:esp32'
	* 
	* But belong to esp32 N16R8 dev-kit-c it also require to add these 
	* following attribute to 
	* the FQBN : esp32:esp32:esp32:FlashSize=16M,PartitionScheme=custom,DebugLevel=none,PSRAM=disabled,LoopCore=1,EventsCore=1,EraseFlash=none,JTAGAdapter=default,ZigbeeMode=default
	* 
	* 


*/

#if !defined( TERMINAL_BAUD )
#define TERMINAL_BAUD_SPEED  115200
#else
#define TERMINAL_BAUD_SPEED  TERMINAL_BAUD
#endif


#if !defined( MASTER_BACKEND_SWITCH )
#define MASTER_BACKEND_VALUE  0
#else
#define MASTER_BACKEND_VALUE  1
#endif

#if MASTER_BACKEND_VALUE == 0
#warning "Audio I2S Backend is disabled" 
#endif 

#include <Arduino.h>
#include <WiFiMulti.h>
#include "secrets.h"  // add WLAN Credentials in here.

//#include <esp32-hal-uart.h>
//#include "/media/maxiste/UBEXTFS240GB-001/Arduino-2.2.1/.arduino15/packages/esp32/tools/esp32-arduino-libs/idf-release_v5.1-33fbade6/esp32/include/driver/uart/include/driver/uart.h"

/* Not working include 
  #include <tools/esp32-arduino-libs/idf-release_v5.1-33fbade6/esp32/include/driver/uart/include/driver/uart.h>
	#include "../../../../../tools/esp32-arduino-libs/idf-release_v5.1-33fbade6/esp32/include/driver/uart/include/driver/uart.h"
	#include "/media/maxiste/UBEXTFS240GB-001/Arduino-2.2.1/.arduino15/packages/esp32/hardware/esp32/3.0.5/cores/esp32/esp32-hal-uart.h"
*/

#if MASTER_BACKEND_VALUE == 1 
#include "Audio.h"
#endif 

#include "SPI.h"
/*
#include "SPIFFS.h"
*/

#include "SD.h"
#include "FS.h"

#define FORMAT_SPIFFS_IF_FAILED true
#define DEFAULT_SERIAL_TIMEOUT	150
#define HOSTNAME "MP3I2STESLADRIVER"

// Digital I/O used
#define SD_CS          5
#define SPI_MOSI      23  
#define SPI_MISO      19
#define SPI_SCK       18
#define I2S_DOUT      25
#define I2S_BCLK      27
#define I2S_LRC       26
#define RX_ESP32_PIN	3
#define TX_ESP32_PIN	1

#define TIMEZONE "EST5EDT"

#if MASTER_BACKEND_VALUE > 0 
Audio audio;
#endif 

WiFiMulti wifiMulti;

//String ssid 				= "Riendeau" ;
//String password 		= "Chloe123!";

String StrReadTerm ; 
String StrCmdRecv  ; 

bool bStateMsgNBE 			= false ;  
bool bStateMsgNoSongAct = false ;  
bool bSetDnsResult 			= false ;
bool bStateEndCmdIn			= false ; 

const int uart_buffer_size = (1024 * 2);

//QueueHandle_t uart_queue;

// optional
#if MASTER_BACKEND_VALUE == 1 
void audio_info(const char *info)
{
  Serial.print("info        "); 
  Serial.println(info);
}

void audio_id3data(const char *info)
{  //id3 metadata
  Serial.print("id3data     ");
  Serial.println(info);
}

void audio_eof_mp3(const char *info)
{  //end of file
  Serial.print("eof_mp3     ");
  Serial.println(info);
}

void audio_showstation(const char *info)
{
  Serial.print("station     ");
  Serial.println(info);
}

void audio_showstreamtitle(const char *info)
{
  Serial.print("streamtitle ");
  Serial.println(info);
}

void audio_bitrate(const char *info)
{
  Serial.print("bitrate     ");
  Serial.println(info);
}

void audio_commercial(const char *info)
{  //duration in sec
  Serial.print("commercial  ");
  Serial.println(info);
}

void audio_icyurl(const char *info)
{  //homepage
  Serial.print("icyurl      ");
  Serial.println(info);
}

void audio_lasthost(const char *info)
{  //stream URL played
  Serial.print("lasthost    ");
  Serial.println(info);
}

#endif 

//esp_err_t	errUartMsg ; 


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

String helper_menu( String StrCmd )
{
  bool isCmdFound = false ;
  String StrMsgRet ; 
  StrCmd.trim() ;
  switch( StrCmd )
  {
		case "help":
			isCmdFound=true ; 
			StrMsgRet="You reach help.\n\r\n\r\n\rThis is help:\n\rChoice 1 : help\n\rChoice 2 : about\n\rChoice 3 : list\n\rChoice 4 : connect\n\rChoice 5 : wifi\n\r\n\r";
			break;

		case "about":
			isCmdFound=true ; 
			StrMsgRet="You reach about.\n\r\n\r\n\rThis is the about.\n\r";
			break;

		case "list":
			isCmdFound=true ; 
			StrMsgRet="";
			break;

		case "connect":
			isCmdFound=true ; 
			StrMsgRet="";
			break;
		
		case "wifi":
			isCmdFound=true ; 
			StrMsgRet="";
			break;

		default: 
			StrMsgRet="Unknow command:[" + StrCmd + "]." ; 
			break;
	}
  return StrMsgRet ; 
}


void setup() 
{
	/*
	 * These lines with ESP_ERROR_CHECK does configure the UART_0 port or the
	 * USB connectec one to esp32 and do assume creating buffer of 2KBytes
	 * or 16Kilo Bits equivalent to a 16550 UART Buffer .  
	 * */
	//ESP_ERROR_CHECK( uart_driver_install(UART_NUM_0, uart_buffer_size, uart_buffer_size, 10, &uart_queue, 0) );
	//ESP_ERROR_CHECK( uart_enable_rx_intr( 0 ) ) ; 
	//ESP_ERROR_CHECK( uart_set_pin(0, TX_ESP32_PIN, RX_ESP32_PIN, -1, -1) ) ; 

	/* Allow 256 byte in reserve for StrReadTerm, apparently it overflow 
	 * every time a += occur to the string and does not add space for. 
	 * SerialStringReader */
	
	StrReadTerm.reserve( uart_buffer_size/8 );

  delay(5000) ;
	/*
	 * Serial reader here. Using attachInterrupt to trigger a function response over 
	 * RX_ESP32_PIN 
	 *
	 * Belong to documentation another esp_32 api command is required and will 
	 * not let the attachInterrupt work by itself:
	 * 
	 * */
	
	/* No attachInterrupt, for RX pin of esp32, the serialEvent does work 
	 * well and do not stop the process withing the loop. 
	 * */
	//attachInterrupt(digitalPinToInterrupt(RX_ESP32_PIN), SerialStringReader, RISING);

  Serial.begin(TERMINAL_BAUD_SPEED);
  while ( ! Serial ) 
  { 
		delay(5) ; 
	}
	
	Serial.setTimeout( DEFAULT_SERIAL_TIMEOUT ) ; 
  //Serial.println( "BOOTING" );
  //Serial.println("MP3 I2S Tesla Driver for esp32.");
  
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  SPI.setFrequency(1000000);
  
  SD.begin(SD_CS);
 
    // start WiFI
  WiFi.mode(WIFI_STA);
  if (strlen(ssid) == 0) 
  {
    WiFi.begin();
  } 
  else 
  {
    WiFi.begin(ssid, passPhrase);
  }
	WiFi.setHostname(HOSTNAME);
  //WiFi.mode(WIFI_STA);
  //wifiMulti.addAP(ssid.c_str(), password.c_str());
  //wifiMulti.run();
  
  if(WiFi.status() != WL_CONNECTED)
  {
		delay(500) ; 
	}
	WiFi.disconnect(true);
	wifiMulti.run();  
	//configTime(TIMEZONE, "pool.ntp.org");
  bSetDnsResult = WiFi.setDNS('24.201.245.77','24.200.243.189') ;
  //Serial.println( "Setting dns respond: " + String( bSetDnsResult ) );

#if MASTER_BACKEND_VALUE == 1 
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(12); // 0...21

  //audio.connecttoFS(SPIFFS, "atr_high_trance_watermark.mp3"); // SPIFFS
  audio.connecttoFS(SD, "atrtrwtm.mp3");
  //    audio.connecttohost("http://www.wdr.de/wdrlive/media/einslive.m3u");
  //    audio.connecttohost("http://somafm.com/wma128/missioncontrol.asx"); //  asx
  //    audio.connecttohost("http://mp3.ffh.de/radioffh/hqlivestream.aac"); //  128k aac
  //    audio.connecttohost("http://mp3.ffh.de/radioffh/hqlivestream.mp3"); //  128k mp3
#else
	//Serial.println("Audio I2S Backend is disabled during internet/serial development.\n\rFor compilation purposes.");
	//Serial.println("Audio I2S Backend is disabled during internet/serial development.\n\rFor compilation purposes.");
#endif 
	

	/*The initial prompt to start typing.*/
	delay(15000) ;
	//Serial.println("") ;  
	//Serial.println("Terminal ready:");
	//Serial.println("Cmd:>");
	
	//Serial.println("Terminal ready:") ; 
	//Serial.print("Cmd:>") ; 
}

void loop()
{

#if MASTER_BACKEND_VALUE == 1 
  audio.loop();
#else
	if( !bStateMsgNBE )
	{ 
		//Serial.println("No audio.loop to process Audio I2S Backend is disabled.");
		bStateMsgNBE = true ;
	} 
#endif 

  //if(Serial.available())
  //{ // put streamURL in serial monitor
#if MASTER_BACKEND_VALUE == 1 
	audio.stopSong();
#else
	if( !bStateMsgNoSongAct ) 
	{
		//Serial.println("No audio.stopSong to process Audio I2S Backend is disabled.");
		bStateMsgNoSongAct=true ;
	} 
#endif 
	//StrReadTerm=Serial.readString(); 
	//StrReadTerm.trim();
	if ( bStateEndCmdIn ) 
	{
		Serial.println( helper_menu( StrCmdRecv )  ) ;
		bStateEndCmdIn=false ;  
		StrCmdRecv="" ; 
	}
	/*if(r.length() > 5 ) 
	{ 
		audio.connecttohost( r.c_str() );
	}*/
    
	//log_i("free heap=%i", ESP.getFreeHeap());
  //}
}

