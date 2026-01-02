#include "LedControl.h"

#ifdef USE_PGM_READ_PTR
#include <avr/pgmspace.h>
#endif

/* Version of october 24 23h50.
 * It compile, but does not produce light from the 8x32 max7219,
 * while it prototype in DisplayMax7219Mtest.ino was initially 
 * based the effort until the move of the dance into a double 
 * array it seems to not affect value correctly.
 * 
 * Used library Version 
 *	LedControl   1.0.6
 *
 * Used platform  Version 
 *	ATTinyCore:avr 1.5.2 
 *  
 * This application work only by compiling it with arduino-cli
 * and does use build.extra_flags compiler.cpp.extra_flag which 
 * can only be set with preferences.txt for uses of Arduino IDE.
 * 
 * 
 * This application use Pointer to verify many problems meet with 
 * library "MATRIX7219.h", where nothing even an external 5x5 font
 * set extracted from old Adafruit_ssd library before they moving 
 * to something apparentl more efficient . I do inspect with another
 * many examples and with "LedControl.h" that assume supporting chip
 * max7219 / max722x. For this I do base my effort on Arduino Space 
 * Invader example, where it re-write all array into one array and
 * it's access are with pointer use. As the example is also discovered 
 * on attiny167 from Digispark called DigisparkPro mini pro is a 
 * single chip with 16KB of memory, 512 Byte of RAM, cheaper than
 * atmel328p it offer something simple to get micronucleus application
 * sending to the chip and assume to be individual acces by a 
 * segment rather than continus access on USB bus making it invisible 
 * ot USB and respond for synchonous attempt until micronucleus take 
 * over the chip. 
 * 
 * As long the problem is not meeted, it answer to a reality the 
 * initial class MATRIX7219.h are not tolerant to pointer or having 
 * relative acces or comunication with static or hard-coded information
 * make this library performing poorly with pointer. Since we haven't 
 * meet problems uniting the array and creating function manipulating 
 * references we can split each invader into key and replace the eight
 * (8) invader image into set of 4 region per square named Left-Upper 
 * Right-Upper, Right-Lower, Left-Lower where we put element forming   
 * invader1a, invader1b and so on. 
 * 
 * This application also use arduino-cli and do use element like 
 * build.extra_flags, compiler.cpp.extra_flag to inform the compilation
 * this application can work telling the time in it during compilation 
 * so once ready to send we are about few seconds before what application
 * is timed in with no much technology that involve predicting and 
 * sending.  
 * 
 * 
 * Relative work with pointer done at the a 32x8 cluster of led 
 * they are max7219 compatible device in a line-up connection.
 * 
 * We do test attiny167 to not answering well to pointer with 
 * other library, we try this out with "LedControl.h" to see 
 * if it's the design of the library access to max7219 that are
 * not well used or properly used. This one is also intended 
 * to optime uses of pointer. Initial image of space invader will
 * be transformer into 4x4 group where original matrix will be draw
 * into 4 key to reduce the array foot-print and store information 
 * of scene componnent into something else like a puzzle part 
 * composition. So don't refer to it to get all the binary matrix
 * to draw space invader they are more accute on the internet. 
 * 
 * */


#define CHAR_DISPLAY_DELAY		 	 25
#define HIGH_MASK_VALUE_COL			256
#define LOW_MASK_VALUE_COL				0
#define TIME_START_AT_COL					2
#define FRAME_COL_LENGTH					8
#define FRAME_ROW_LENGTH					8
#define ANIMATION_SPRITE_NUMBER	 18
#define BOOT_DELAY						 3000

/*
 * This section require a -DHAVE_INTERNAL_HARD_SET inside 
 * build.extra_flags compiler.cpp.extra_flag which can also
 * be set inside preferences.txt. This is bad for value 
 * that can change upon compilation like time addressing. 
 * */

/* Uncomment HAVE_INTERNAL_HARD_SET to let arduino ide 
compile it online 
*/ 
#define	HAVE_INTERNAL_HARD_SET		1

#ifdef HAVE_INTERNAL_HARD_SET
#define HARD_HOUR_VAL					10
#define HARD_MIN_VAL					10
#define HARD_SEC_VAL					10
#endif

/*
 * This section require a -DHAVE_INTERNAL_MATRIX_COUNT inside 
 * build.extra_flags compiler.cpp.extra_flag which can also
 * be set inside preferences.txt. This is not especially 
 * bad for this case where there almost only 1 square example
 * and 4 squares sold on retail market. Using 
 * HAVE_INTERNAL_MATRIX_COUNT throught compilation line or 
 * changing it with preferences.txt make  INTERNAL_MATRIX_COUNT
 * working here inside the code. Not using it will signal there 
 * is no known of  INTERNAL_MATRIX_COUNT, so you have to add it inside
 * arduino-cli compile \ 
 * 	--build-property "build.extra_flags=-DHAVE_INTERNAL_MATRIX_COUNT=1" \ 
 * 	--build-property "compiler.cpp.extra_flag=-DHAVE_INTERNAL_MATRIX_COUNT=1" 
 * 
 * But because you do have an acces to arduino-cli just tell it's 4 block
 * arduino-cli compile \
 *  --build-property "build.extra_flags=-DINTERNAL_MATRIX_COUNT=4" \
 *  --build-property "compiler.cpp.extra_flag=-DINTERNAL_MATRIX_COUNT=4"
 *  
 * */

/* Uncomment HAVE_INTERNAL_MATRIX_COUNT to let arduino ide 
compile it online 
*/ 
#define	HAVE_INTERNAL_MATRIX_COUNT		1

#ifdef HAVE_INTERNAL_MATRIX_COUNT
#define INTERNAL_MATRIX_COUNT					4
#endif

/* 
 * Uncomment HAVE_INTERNAL_PGM_PTR_CONF to let arduino ide 
 * compile it online 
*/ 
//#define	HAVE_INTERNAL_PGM_PTR_CONF		1

#ifdef HAVE_INTERNAL_PGM_PTR_CONF
#define 		USE_PGM_READ_PTR		1
#endif

uint8_t dataPin   = 10;
uint8_t selectPin = 12;
uint8_t clockPin  = 11;
uint8_t count     = INTERNAL_MATRIX_COUNT;

// Pins: DIN,CLK,CS, # of Display connected
LedControl lc = LedControl( dataPin, clockPin, selectPin, count);  


// Put values in arrays
//byte invader1a
/*
 * All the invader inserted in one Buffer.
 * 
 * Called BufferImage, it's decl. does coun't an 8 x 8 matrix where 
 * the library LedControl.h help us drawing by square and by segment of 
 * buffer. 
 * 
 * The goal of that sketches, let use space invader in background with 
 * appropriate space available in front to publish hour ,min, sec. 
 * The clock will pass by every 5 or 10 seconds or simply roll-on .
 * 
 * Time segment like number , am/pm sign for the clock are in 5x5 bit and 
 * storing here will be to overuse space of 64 bytes for reservate 25 bits.
 * Rounded by the compiler it store that 25 bit into 32 bit or 4 char. 
 * So it's strongly possible to inherit of 2 segments available within
 * 4 chars and up to 8 segment (like 0 - to 7) in one segment of 64 bit long
 * as suggested space declared inside BufferImage. 
 * 
 * How Invader are stored  ?
 * - stored long after united all the invader array named from their name 
 * used in a long declaration of array to be draw after the other. Merging 
 * everything inside BufferImage, make it easy to add other image and create
 * scene.
 * We kept the memory name of the scene by adding number of the scene index
 * reference inside an enum enum_FrameName which become a type def for our
 * use, typedef eFrameName. 
 * 
 * To acces to a member of this array, at least image in 8x8 not 5x5 is 
 * implying to use a pointer FrameImage which use malloc allocation and 
 * magic of pointer to acces on and on to all member in a form :
 * 
 * ptrByteField = *( BufferImage + eNameFrameSel ) ;
 * 
 * Where  eNameFrameSel is the enum eFrameName that make easy way to 
 * call the image by it's name.  
 * 
 * Finally the form to allow accessing it make sense using DrawRowSet and
 * FrameReturnPtr all together . 
 * 
 * Function DrawRowSet will ask to a specific blok to receive the matrix of
 * your design into pointer returned by FrameReturnPtr, where this one hold 
 * about the FrameImage malloc reference to make the magic of pointer calling
 * the affectation inside BufferImage[8][8]. Which is not accessing array in a 
 * form of Array[x][y] -> new variable ; you to have to pass by this set of 
 * FrameReturnPtr, as correct way to tell you with what big your are going to 
 * draw and which scene in the same enum previously says. 
 * ( 0, FrameReturnPtr( FrameImage, invader1a )
 * 
 * example of accessing to Square0 to draw  invader1a, and on Square1 draw 
 * the invader1b to look like rolling by turn to step up .
 * 
 * DrawRowSet( 0, FrameReturnPtr( FrameImage, invader1a ) , ( unsigned long )( CHAR_DISPLAY_DELAY ) ) ;
 * DrawRowSet( 1, FrameReturnPtr( FrameImage, invader1b ), ( unsigned long )( CHAR_DISPLAY_DELAY )) ;
 * 
 * As number are compatible to fit in a 8 interger long definitions
 * enum eFrameName hold address letter1, letter2, letter3, to hold up 
 * to 24 char in a three section. This mean FrameReturnPtr( FrameImage,letter1 )
 * will point to the address where 8 display number can be here, and another 
 * function will extract the letter and the other one will draw it.
 * 
 * */

typedef enum enum_FrameName 
{ 
	invader1a= 0,
	invader1b= 1,
	invader2a= 2,
	invader2b= 3,
	invader3a= 4,
	invader3b= 5,
	invader4a= 6,
	invader4b= 7,
	letter1=   8,
	letter2=   9,
	letter3=  10
} eFrameName ; 

//typedef enum_FrameName eFrameName ; 

//eFrameName FrameName ; 

/*
 * struct st_BlockToImage
 * raised into typedef struct st_BlockToImage stBlockMAX7219_type
 * to make stBlockMAX7219_type an easy structure with pointer to 
 * call the prt->[ iBlockid | eFrameName | xiDecal | yiDecal | iLetterRegister ]
 * to make easy aceesing to information . 
 * Sprite will be eiter invader... and letter where invader are in format
 * 8x8 and letter 5x5, this struct will own xiDecal/yiDecal for transition 
 * of element 5x5 in the center of the 8x8 block and will sometime been reffer
 * to an 3x3 where prt->[xiDecal|yiDecal] will own the information and possibly
 * iLetterRegister member to hold the row position where the letter is pointed
 * as register 0/40 rows possible. Assuming it's letter in 5x5 we store 8 
 * characters in the 8x8 BufferImage row. 
 * */

typedef struct st_BlockToImage 
{ 
	int iBlockid ;
  eFrameName eName;
  int xiDecal=0;
  int yiDecal=0 ;
  int iLetterRegister ; 
} stBlockMAX7219_type;

//typedef struct st_BlockToImage stBlockMAX7219_type ;

/*
 * Original invader dance let display all four pseudo-posing.
 * 
 * information on TableScene1 
 * Having originally block position and invader-sprite-name, 
 * I added the typedef stBlockMAX7219_type which is a structure
 * of 5 member where id eName is an enum which is suppose to be 
 * interger like definition. This is also why we enforce the casting 
 * in integer here. Because it's possible to put declaration of structure
 * here, I will recompose the TableScene1 into a 18 structures of 
 * typedef stBlockMAX7219_type, and can possibly pass this time. 
 * 
 * 
 * */
 
int TableScene1[ANIMATION_SPRITE_NUMBER	* 5 ] = 
	{   0,(int)(invader1a),0,0,0,
		  1,(int)(invader1b),0,0,0,
		  0,(int)(invader2a),0,0,0,
		  1,(int)(invader2b),0,0,0,
		  2,(int)(invader1a),0,0,0,
		  3,(int)(invader1b),0,0,0,
		  2,(int)(invader2a),0,0,0,
		  3,(int)(invader2b),0,0,0,
		  0,(int)(invader3a),0,0,0,
		  1,(int)(invader3a),0,0,0,
		  0,(int)(invader4b),0,0,0,
		  1,(int)(invader4b),0,0,0,
		  2,(int)(invader4a),0,0,0,
		  3,(int)(invader4a),0,0,0,
		  2,(int)(invader4b),0,0,0,
		  3,(int)(invader4b),0,0,0,
		  2,(int)(invader3b),0,0,0,
		  3,(int)(invader3b),0,0,0  
	} ; 

/*
 * Between all invader image available we setup an enumeration for
 * eventually develop random image base by a call of random with 
 * available image, and by group ; leaving random calling a 
 * invader1a range to invader1b to see only a clap in the air
 * or declapping from the aire to back to it's state will be the 
 * result of a field ( invader1a, invader1b) , or declapping with
 * field ( invader1b, invader1a)
 * */

#ifdef USE_PGM_READ_PTR
static const byte BufferImage[FRAME_ROW_LENGTH][FRAME_COL_LENGTH] PROGMEM =
#else
static byte BufferImage[FRAME_ROW_LENGTH][FRAME_COL_LENGTH] =
#endif
{
	// invader1a -- equivalent to Index : 0
	B00011000,B00111100,B01111110,B11011011,B11111111,B00100100,B01011010,B10100101, // First frame of invader #1
	// invader1b -- equivalent to Index : 1
	B00011000,B00111100,B01111110,B11011011,B11111111,B00100100,B01011010,B01000010, // Second frame of invader #1
	// invader2a -- equivalent to Index : 2
	B00100100,B00100100,B01111110,B11011011,B11111111,B11111111,B10100101,B00100100, // First frame of invader #2
	// invader2b -- equivalent to Index : 3
	B00100100,B10100101,B11111111,B11011011,B11111111,B01111110,B00100100,B01000010, // Second frame of invader #2
	// invader3a -- equivalent to Index : 4
	B00011000,B00111100,B01111110,B11011011,B11111111,B00100100,B01011010,B10100101, // First frame of invader #1
	// invader3b -- equivalent to Index : 5
	B00011000,B00111100,B01111110,B11011011,B11111111,B00100100,B01011010,B01000010, // Second frame of invader #1
	// invader4a -- equivalent to Index : 6
	B00100100,B00100100,B01111110,B11011011,B11111111,B11111111,B10100101,B00100100, // First frame of invader #2
	// invader4b -- equivalent to Index : 7
	B00100100,B10100101,B11111111,B11011011,B11111111,B01111110,B00100100,B01000010, // Second frame of invader #2
} ;

byte *FrameImage = NULL ; 

int *PSceneRnd = NULL ;

int icAct=0 ;  

void *FunctionCalling = NULL ; 

//void *PtrFuncCallRet 	= NULL ; 

//stBlockMAX7219_type *BlockConf = NULL ; 

stBlockMAX7219_type *SampleConfScene = NULL ;  

/*
 * Function declaration bloc required to generate a 
 * 
 * void (*SpriteAction[])() = { InvaderDance, InvaderPausing}; 
 * 
 * Allowing to set some action run one after the other, and eventually 
 * define a set of action thru the building environment like build.extra_flags/
 * compiler.cpp.extra_flag added inside --build-property, and will issues 
 * a compiling option to add individual action at following of this pre-defined
 * existence of that [void (*)()] void type in between bracket. 
 * 
 * 
 * */

void ProcessFunc( void (*func)(void)) ; 
void InvaderDance( void ) ; 
void InvaderPausing( void ) ; 
void ExchangePosing( void ) ; 
//void ConfigureDance( int *ptrArrayScene ) ;
void ConfigureDance( void ) ; 
void DrawRowSet( int intBlockRow, byte *bMatrixSet, unsigned long ldelayTime, bool isDelay=true ) ; 
byte* FrameReturnPtr( byte *ptrByteField, eFrameName eNameFrameSel ) ; 


/*
 * Virtual void of virtual element known to be function and 
 * will be called once-after the other as demonstration of 
 * filling loop with another loop of that successive action.
 * 
 * Apparently it require to be initialized after only prototype declaration,
 * or example out of arduino does not see the content unless prototype exist. 
 * 
 * */
void (*SpriteAction[])( ) = { InvaderDance, InvaderPausing , ExchangePosing }; 

void ProcessFunc( void (*func)(void)) 
{
  return func( );
}

void DrawRowSet( int intBlockRow, byte* bMatrixSet, unsigned long ldelayTime, bool isDelay=true )
{

	/* 
	 * DrawRowSet information :
	 * void DrawRowSet( int intBlockRow, byte* bMatrixSet, unsigned long ldelayTime, bool isDelay=true )
	 * 
	 */
  for (int i = 0; i < FRAME_ROW_LENGTH; i++)
  {
    lc.setRow(intBlockRow,i, *( bMatrixSet + i));
  }
  if( isDelay )
  {
		delay(ldelayTime); 
	}
}

byte *FrameReturnPtr( byte* ptrByteField, eFrameName eNameFrameSel )
{
#ifdef USE_PGM_READ_PTR 
	ptrByteField = (byte*)( pgm_read_byte( &BufferImage + eNameFrameSel )  ) ;
#else
	ptrByteField = (byte*)( &BufferImage[ eNameFrameSel ]  ) ;
#endif
	return ptrByteField ;
}


void ConfigureDance( void )
{
	
	//BlockConf = malloc( sizeof( stBlockMAX7219_type ) * 1 ) ;
	//int iCountBlk=0 ; 
	for( int iAddrBlk = 0 ; iAddrBlk < ANIMATION_SPRITE_NUMBER * 5 ; iAddrBlk+=5 )
	{
		stBlockMAX7219_type *BlockConf = ( stBlockMAX7219_type *)( TableScene1 + iAddrBlk )  ;
		//BlockConf-> iBlockid = *( TableScene1 + iAddrBlk ) ; 
		//BlockConf-> eName = (int)( TableScene1 + (iAddrBlk+1) ) ;
		// a matrix of 8x8 require (0,0) to not overflow everywhere. 
		//BlockConf->xiDecal= 0 ; 
		//BlockConf->yiDecal= 0 ; 
		//BlockConf->iLetterRegister= 0 ;
		(SampleConfScene + iAddrBlk)-> iBlockid = BlockConf->iBlockid ;
		(SampleConfScene + iAddrBlk)-> eName = BlockConf->eName ; 
		(SampleConfScene + iAddrBlk)-> xiDecal = BlockConf->xiDecal ;
		(SampleConfScene + iAddrBlk)-> yiDecal = BlockConf->yiDecal ;
		(SampleConfScene + iAddrBlk)-> iLetterRegister = BlockConf->iLetterRegister ;
		//free(BlockConf) ; 
	}
}

void InvaderDance( void )
{
	// First Set 
	// Put #1 frame on both Display

	unsigned long ulWaitDelay = ( unsigned long )( CHAR_DISPLAY_DELAY ) ; 
	byte *bPairDraw = NULL ; 
	
	bPairDraw = (byte*)( malloc ( sizeof( byte ) * 2 ) ) ;

	for( int iCount = 0 ; iCount < 100 ; iCount++ )
	{	
		for( int iAddrBlk=0 ; iAddrBlk < ANIMATION_SPRITE_NUMBER ; iAddrBlk+=2 )
		{
			*(bPairDraw+0) = FrameReturnPtr( FrameImage, (eFrameName)((SampleConfScene+iAddrBlk)->eName) ) ; 
			*(bPairDraw+1) = FrameReturnPtr( FrameImage, (eFrameName)((SampleConfScene+(iAddrBlk+1))->eName) ) ;  
			DrawRowSet( (int)( ( SampleConfScene + iAddrBlk )->iBlockid ) 		  , 
									(bPairDraw+0) , 
									ulWaitDelay ) ;
			DrawRowSet( (int)( ( SampleConfScene + (iAddrBlk + 1) )->iBlockid )  , 
									(bPairDraw+1), 
									ulWaitDelay ) ;
		}
	}
}

/*
 * 2 spaces invaders pausings , 2 others crawlings.
 * */
void InvaderPausing( void )
{
	unsigned long ulWaitDelay = ( unsigned long )( CHAR_DISPLAY_DELAY ) ;  
	for( int iCount=0 ; iCount < 50 ; iCount++ )
	{	
		for( int iScene=0 ; iScene < 10 ; iScene ++ )
		{
			*(PSceneRnd) = random( invader1a, invader2b ) ;
			*(PSceneRnd+1) = random( invader2a, invader2b ) ;
			*(PSceneRnd+2) = random( invader3a, invader3b ) ;
			*(PSceneRnd+3) = random( invader4a, invader4b ) ;
			
			for( int iGroup=0 ; iGroup < INTERNAL_MATRIX_COUNT  ; iGroup++ )
			{
				if( iGroup == 0 )
				{
					DrawRowSet( iGroup, FrameReturnPtr( FrameImage, (int)(*(PSceneRnd+(iGroup))) ), ulWaitDelay) ;
				}
				if( iGroup == 1 )
				{
					DrawRowSet( iGroup, FrameReturnPtr( FrameImage, (int)(*(PSceneRnd+(iGroup-1))) ), ulWaitDelay) ;
				}
			}
		}
	}
}

void ExchangePosing( void )
{
	unsigned long ulWaitDelay = ( unsigned long )( CHAR_DISPLAY_DELAY ) ;  
	for( int iCount=0 ; iCount < 50 ; iCount++ )
	{	
		for( int iScene=0 ; iScene < 10 ; iScene ++ )
		{
			*(PSceneRnd) = random( invader4a, invader4b ) ;
			*(PSceneRnd+1) = random( invader3a, invader3b ) ;
			*(PSceneRnd+2) = random( invader2a, invader2b ) ;
			*(PSceneRnd+3) = random( invader1a, invader1b ) ;
			
			for( int iGroup=2 ; iGroup < 4  ; iGroup++ )
			{
				if( iGroup == 2 )
				{
					DrawRowSet( iGroup, FrameReturnPtr( FrameImage, (int)(*(PSceneRnd+(iGroup-1))) ), ulWaitDelay) ;
				}
				if( iGroup == 3 )
				{
					DrawRowSet( iGroup, FrameReturnPtr( FrameImage, (int)(*(PSceneRnd+(iGroup-2))) ), ulWaitDelay) ;
				}
			}
		}
	}
}

void setup()
{
	Serial.begin(115200);
	delay( BOOT_DELAY );
	pinMode(dataPin, OUTPUT);
  pinMode(selectPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
	
	for( int iSetup=0 ; iSetup < INTERNAL_MATRIX_COUNT; iSetup++ )
	{
		lc.shutdown(iSetup,false);  // Wake up displays
		lc.setIntensity(iSetup,5);  // Set intensity levels
		lc.clearDisplay(iSetup) ;
	}
	/*
   * Generate for equal amount of block/matric as INTERNAL_MATRIX_COUNT is set
   * to offer random scene information as pointer. to the reference for later use 
   * inside function loop(). 
   * */
     
  PSceneRnd = malloc( sizeof( int ) * INTERNAL_MATRIX_COUNT ) ;
  
  //stBlockMAX7219_type *SampleConfScene ; 
	//SampleConfScene = static_cast< stBlockMAX7219_type* >( malloc( sizeof( stBlockMAX7219_type ) * 36  ) ) ;
  //SampleConfScene = ( stBlockMAX7219_type* )( malloc( sizeof( stBlockMAX7219_type ) * 36  ) ) ;
  SampleConfScene = (stBlockMAX7219_type*)( malloc( sizeof( stBlockMAX7219_type ) * ANIMATION_SPRITE_NUMBER  ) ) ;
  
  /*Forcing having a malloc over FrameImage, was initially inside loop, 
   * where after moving in a if clause make the matrix showing nothing,
   * or other modification when encanistation of a long list of DrawRowSet
   * was transformed into a loop. So I add one here 
   * */
  FrameImage = (byte*)( malloc( sizeof( byte ) * FRAME_ROW_LENGTH ) ) ;
  
  //ConfigureDance( TableScene1 ) ;  
	ConfigureDance( ) ; 
  
}

void loop()
{

		for( int icAct=0; icAct < sizeof( *SpriteAction ); icAct++)
		{
#ifdef FULL_RNTRPRT_CAST_ON_PTRFUNC
			FunctionCalling = reinterpret_cast< void*>( SpriteAction + icAct ) ;
			ProcessFunc( reinterpret_cast< void (*)(void) >(FunctionCalling) ) ;			
#else
			FunctionCalling = *( SpriteAction + icAct )  ;  
			ProcessFunc( FunctionCalling ) ;
#endif	
 
		}

}
