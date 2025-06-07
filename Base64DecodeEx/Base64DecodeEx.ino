#if !defined( TERMINAL_BAUD )
#define TERMINAL_BAUD_SPEED  115200
#else
#define TERMINAL_BAUD_SPEED  TERMINAL_BAUD
#endif

#include <Arduino.h>
#include "mbedtls/base64.h"

//#include "SD.h"
//#include "FS.h"

#define DEFAULT_SERIAL_TIMEOUT  150
#define HOSTNAME                "BASE64DECODEEXAMPLE"
#define TIME_OUT_TEST           5000

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
 * Initial information, in base64  length = 1512 
 *                         decoded length = 1133
 * 
 */
String StrMsgB64Menu = "VGhlIEhFTFAgU2VjdGlvbi4NCg0KMS0gaGVscA0KCVRoaXMgaXMgdGhpcyBoZWxwLg0KMi0gYWJvdXQNCglUaGlzIGlzIGFuIEFyZHVpbm8gcHJvamVjdCBjb21waWxlZCB3aXRoDQoJQXJkdWlub0lERSAyLjIuMS1hcm02NCBvciBhcmR1aW5vLWNsaQ0KCVZlcnNpb246IDEuMS4xIENvbW1pdDogZmE2ZWFmY2IuIFVzZQ0KCW1vc3RseSBlc3AzMiBsaWJyYXJ5IHZlcnNpb24gMy4wLjUuDQozLSBsaXN0DQoJTGlzdCB0aGUgY29udGVudCBvZiB5b3VyIFNEL1NJUEZGUy9GQVQNCglvbiBlc3AzMiBvciBTRGNhcmQuDQoJU3ludGF4OiBsaXN0IFtTRC9TUElGRlNdDQo0LSBvcGVuDQoJT3BlbiBhIG1lZGlhIG9uIFNEL1NQSUZGUyBkZXZpY2Ugb3INCglTRGNhcmQuDQoJU3ludGF4OiBvcGVuIFtTRC9TUElGRlNdIEZJTEVfTkFNRQ0KNS0gY29ubmVjdA0KCU9wZW4gYW4gVVJJL1VSTCBsb2NhdGlvbiBmb3IgcGxheWluZy4NCglTeW50YXg6IGNvbm5lY3QgW1VSTC9VUkldDQo2LSBzdG9wDQoJU3RvcCBhY3R1YWwgcGxheWluZy4NCglTeW50YXg6IHN0b3ANCjctIHBsYXkNCglTdGFydCBhIHBsYXkgaWYgaXMgc3RvcHBlZC4NCglTeW50YXg6IHBsYXkNCjgtIHdpZmkNCglXaWZpIHNlY3Rpb24sIHRoaXMgaXMgcG9zc2libGUgdG8NCglyZS1jb25maWd1cmUsIHF1ZXJ5IHRoZSB3aWZpIHBhcmFtZXRlcg0KCWFuZCBnZXQgaXQncyBjb25maWd1cmF0aW9uLg0KCVN5bnRheDoNCgl3aWZpIFtvbi9vZmZdIC0gVHVybiBvbi9vZmYgdGhlIHdpZmkuDQoJd2lmaSBjaGFuZ2UgW3NzaWQvcGFzcy9ob3N0XSAtIHNldCBuZXcNCgkJY29ubmVjdGlvbiBvciBjaGFuZ2UgdGhlIGhvc3RuYW1lLg0KCXdpZmkgc2NhbiAtIFNjYW4gd2lmaSBmb3IgQlNTSUQvU1NJRCBuZXR3b3JrLg0KCXdpZmkgcXVlcnkgW2lwL21hYy9kbnNdIC0gb2J0YWluIGluZm9ybWF0aW9ucy4NCjktIHBpbmcNCglQaW5nIGFuIElQIGFkZHJlc3MuDQoJU3ludGF4OiBwaW5nIElQX0FERFINCjEwLSB0ZWxuZXQNCglBY2Nlc3MgdG8gdGVsbmV0IG9yIGNvbW1hbmQgdGVybWluYWwuDQoJU3ludGF4OiB0ZWxuZXQgVVJML0lQX0FERFIgWzAtMTAyNF0NCjExLSByZWJvb3QNCglSZXN0YXJ0IHRoZSBNUDNJMlNURVNMQURSSVZFUiBob3N0Lg0KDQo=" ; 

void setup() 
{
  delay( TIME_OUT_TEST ) ;
	
  Serial.begin(TERMINAL_BAUD_SPEED);
  while ( ! Serial ) 
  { 
		delay( TIME_OUT_TEST / 1000 ) ; 
	}
	Serial.setTimeout( DEFAULT_SERIAL_TIMEOUT ) ; 
  
  Serial.println( "BOOTING" );
  Serial.println("Base64Decode Example");
  Serial.println("Showing you the translated base64 menu.");

	delay(TIME_OUT_TEST) ;
	
	size_t outlen=0;

	mbedtls_base64_decode( NULL, 
	                       0, 
	                       &outlen, 
	                       (const unsigned char*)StrMsgB64Menu.c_str(), 
	                       StrMsgB64Menu.length());
	
	unsigned char* decoded_output = (unsigned char*)malloc(outlen);
	
	mbedtls_base64_decode( decoded_output, 
	                       outlen, &outlen, 
	                       (const unsigned char*)StrMsgB64Menu.c_str(), 
	                       StrMsgB64Menu.length());
	

	
	Serial.printf( "%s\r\n", (char*)( decoded_output ) );
	free(decoded_output) ;
}

void loop()
{

	/*
	 * More code to come. 
	 * 
	 * */

	delay(TIME_OUT_TEST/100) ; 

	/*
	 * even more there's too. 
	 * 
	 * */

}



